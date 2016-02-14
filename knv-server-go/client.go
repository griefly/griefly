package main

import (
	"log"
	"math/rand"
	"strconv"
	"time"
)

const (
	PlayerQueueLength   = 32
	RegistryQueueLength = 8
)

var (
	TickInterval        = 100 * time.Millisecond
	StartHashCheckEvery = 5
	HashCheckTimeout    = 500 * time.Millisecond
)

type newPlayerReply struct {
	id             int
	master         bool
	mapDownloadURL string
	inbox          chan *Envelope
}

type PlayerEnvelope struct {
	m        *Envelope
	response chan newPlayerReply
}

type PlayerInfo struct {
	id    int
	login string
}

type versionCheck struct {
	version  string
	response chan versionCheckResponse
}

type versionCheckResponse struct {
	ok      bool
	version string
}

type hashCheck struct {
	deadline           time.Time
	waitingClientCount int
	hashes             map[int]int
}

func (hc *hashCheck) add(id int, msg *MessageHash) {
	hc.hashes[id] = *msg.Hash
	hc.waitingClientCount--
}

type playerDrop struct {
	id     int
	reason *Envelope
}

type RegistryConfig struct {
	DumpRoot string
}

type Registry struct {
	nextID  int
	clients map[int]chan *Envelope
	players map[string]PlayerInfo

	masterID      int
	clientVersion string

	newPlayers   chan PlayerEnvelope
	versionCheck chan versionCheck

	droppedPlayers chan playerDrop

	hashes      map[int]*hashCheck
	currentTick int

	inbox       chan *Envelope
	ticker      *time.Ticker
	assetServer *AssetServer
	dumper      *DumpWriter

	config *RegistryConfig
}

func newRegistry(as *AssetServer, config *RegistryConfig) *Registry {
	return &Registry{1, make(map[int]chan *Envelope, RegistryQueueLength), make(map[string]PlayerInfo),
		-1, "", make(chan PlayerEnvelope), make(chan versionCheck), make(chan playerDrop),
		make(map[int]*hashCheck), 0, make(chan *Envelope), nil, as, nil, config}
}

// async api
func (r *Registry) CreatePlayer(m *Envelope) (inbox chan *Envelope, id int, master bool, mapURL string) {
	pe := PlayerEnvelope{m, make(chan newPlayerReply, 1)}
	r.newPlayers <- pe
	response := <-pe.response
	return response.inbox, response.id, response.master, response.mapDownloadURL
}

func (r *Registry) RemovePlayer(id int, reason *Envelope) {
	r.droppedPlayers <- playerDrop{id, reason}
}

func (r *Registry) RecvMessage(e *Envelope) {
	r.inbox <- e
}

func (r *Registry) CheckVersion(version string) (bool, string) {
	req := versionCheck{version, make(chan versionCheckResponse, 1)}
	r.versionCheck <- req
	reply := <-req.response
	return reply.ok, reply.version
}

// internals
func (r *Registry) sendAll(m *Envelope) {
	var deadPlayers []int
	for idx := range r.clients {
		if !r.sendOne(idx, m) {
			deadPlayers = append(deadPlayers, idx)
		}
	}

	for _, idx := range deadPlayers {
		r.removePlayer(idx, &Envelope{&ErrmsgTooSlow{}, MsgidTooSlow, 0})
	}
}

func (r *Registry) sendOne(idx int, m *Envelope) bool {
	client := r.clients[idx]
	if client == nil {
		return false
	}

	select {
	case client <- m:
		return true
	default:
		return false
	}
}

func (r *Registry) sendMaster(m *Envelope) {
	if len(r.clients) == 0 {
		log.Fatal("registry: cannot send to master when there are no clients at all")
	}
	r.clients[r.masterID] <- m
}

func (r *Registry) Run() {
	r.ticker = time.NewTicker(TickInterval)

	for {
		select {
		case m := <-r.inbox:
			if r.handleHash(m) {
				continue
			}

			r.handleGameMessage(m)

		case now := <-r.ticker.C:
			r.handleNewTick()
			r.checkForHashStart(now)
			r.checkHashes(now)

		case newPlayer := <-r.newPlayers:
			r.checkForNewGame()
			r.registerPlayer(newPlayer)

		case info := <-r.droppedPlayers:
			r.removePlayer(info.id, info.reason)

		case req := <-r.versionCheck:
			r.handleVersionCheck(req)
		}
	}
}

func (r *Registry) checkForNewGame() {
	if len(r.clients) != 0 {
		return
	}

	// reset state for new game
	r.currentTick = 0
	sessionID := time.Now().String()
	r.dumper = NewDumpWriter(r.config.DumpRoot, sessionID)
}

func (r *Registry) registerPlayer(newPlayer PlayerEnvelope) {
	m := newPlayer.m.Message.(*MessageLogin)
	var id int

	if m.IsGuest {
		// generate new guest user
		m.Login = newGuest(r, m.Login)
	}

	// TODO(mechmind): authenticate players

	// look up for existing player avatar for current map
	if info, ok := r.players[m.Login]; ok {
		id = info.id
		log.Printf("registry: reusing existing id %d for player '%s'", id, m.Login)
		// remove player and then add him again
		r.removePlayer(id, nil)
		if _, ok := r.players[m.Login]; !ok {
			// server was restarted, so restart registration process
			r.registerPlayer(newPlayer)
			return
		}

		info = PlayerInfo{id: id, login: m.Login}
		r.players[m.Login] = info
	} else {
		id = r.nextID
		r.nextID++
		info = PlayerInfo{id: id, login: m.Login}
		r.players[m.Login] = info
		log.Printf("registry: registered new id %d for player '%s'", id, m.Login)

		// create player on maps
		newm := &MessageNewClient{&id}
		e := &Envelope{newm, MsgidNewClient, 0}
		r.sendAll(e)
		log.Printf("registry: creating new unit for client %d, login '%s'", id, m.Login)
	}

	// if there are queue for previous connection of client then close it
	if r.clients[id] != nil {
		close(r.clients[id])
	}

	// create inbox for client
	inbox := make(chan *Envelope, PlayerQueueLength)
	r.clients[id] = inbox

	var master bool
	var mapDownloadURL string
	if r.masterID == -1 {
		// praise the new master!
		r.masterID = id
		master = true
	} else {
		// immediately request map for new player
		var mapUploadURL string
		_, mapUploadURL, mapDownloadURL = r.assetServer.MakePipe()
		curTick := r.currentTick
		m := &MessageMapUpload{&curTick, mapUploadURL}
		e := &Envelope{m, MsgidMapUpload, 0}
		log.Printf("registry: requesting master %d to send map", r.masterID)
		r.sendMaster(e)
	}

	response := newPlayerReply{id: id, master: master, mapDownloadURL: mapDownloadURL, inbox: inbox}
	newPlayer.response <- response
}

func (r *Registry) removePlayer(id int, reason *Envelope) {
	inbox, ok := r.clients[id]
	if !ok {
		// already deleted
		return
	}

	// send reason
	if reason != nil {
		r.sendOne(id, reason)
	}

	close(inbox)
	delete(r.clients, id)

	// if we deleted master, reelect new one
	if r.masterID == id {
		r.masterID = -1
		for id := range r.clients {
			r.masterID = id
			break
		}
	}

	if r.masterID == -1 {
		// there are no players left, clean up everything
		log.Println("registry: no players left, cleaning up")
		r.cleanUp()
	}
}

func (r *Registry) handleHash(m *Envelope) bool {
	hash, ok := m.Message.(*MessageHash)
	if !ok {
		return false
	}

	checker := r.hashes[*hash.Tick]
	// too late hash check
	if checker == nil {
		return true
	}

	checker.add(m.From, hash)

	return true
}

func (r *Registry) handleGameMessage(m *Envelope) {
	frv, ok := m.Message.(Forwardable)
	if !ok {
		log.Printf("registry: cannot set id for message while forwarding it, dropping. "+
			"ID: %d, type: %s", m.From, m.Message.TypeName())
		return
	}

	frv.SetID(m.From)
	r.sendAll(m)
}

func (r *Registry) handleNewTick() {
	r.currentTick++
	m := &MessageNewTick{}
	e := &Envelope{m, MsgidNewTick, 0}
	r.sendAll(e)
}

func (r *Registry) checkForHashStart(now time.Time) {
	if r.currentTick%StartHashCheckEvery != 0 || len(r.clients) < 2 {
		return
	}

	checker := &hashCheck{
		deadline:           now.Add(HashCheckTimeout),
		hashes:             make(map[int]int),
		waitingClientCount: len(r.clients),
	}

	r.hashes[r.currentTick] = checker
	// request hashes from all clients
	tick := r.currentTick
	m := &MessageRequestHash{&tick}
	e := &Envelope{m, MsgidRequestHash, 0}
	r.sendAll(e)
}

func (r *Registry) checkHashes(now time.Time) {
	for tick, checker := range r.hashes {
		if checker.deadline.Before(now) {
			// deadline expired for this hash check, force check
			r.checkHashesOne(tick, checker)
			delete(r.hashes, tick)
		} else if checker.waitingClientCount <= 0 {
			// we collected hashes from all clients, start check
			r.checkHashesOne(tick, checker)
			delete(r.hashes, tick)
		}
	}
}

func (r *Registry) checkHashesOne(tick int, checker *hashCheck) {
	master := r.masterID
	if master == -1 {
		// all of sudden we have no players, so nothing to do
		return
	}

	mhash, ok := checker.hashes[master]
	if !ok {
		// master is slow, very very slow
		log.Printf("hash-check: master %d did not sent his hash", master)
		return
	}
	delete(checker.hashes, master)

	unsynced := []int{}
	for id, hash := range checker.hashes {
		if hash != mhash {
			unsynced = append(unsynced, id)
		}
	}

	if len(unsynced) == 0 {
		// everyone in sync, OK
		return
	}

	// request and dump map from master and all of those faulty clients
	r.dumpPlayerMap(master, tick, func() {})
	for _, neud := range unsynced {
		id := neud
		callback := func() {
			r.RemovePlayer(id, &Envelope{&ErrmsgOutOfSync{}, MsgidOutOfSync, 0})
		}

		r.dumpPlayerMap(id, tick, callback)
	}
}

func (r *Registry) dumpPlayerMap(id, tick int, callback func()) {
	pipe, uploadURL, _ := r.assetServer.MakePipe()
	mapreq := &MessageMapUpload{&tick, uploadURL}
	msg := &Envelope{mapreq, MsgidMapUpload, 0}
	if !r.sendOne(id, msg) {
		// too slow to live
		r.removePlayer(id, &Envelope{&ErrmsgTooSlow{}, MsgidTooSlow, 0})
	}

	// start dumper
	uid := strconv.Itoa(id)
	if id == r.masterID {
		uid += "-master"
	}

	go r.dumper.DumpMap(uid, pipe, callback)
}

func (r *Registry) handleVersionCheck(req versionCheck) {
	if r.clientVersion == "" {
		r.clientVersion = req.version
		req.response <- versionCheckResponse{true, req.version}
	} else {
		req.response <- versionCheckResponse{req.version == r.clientVersion, r.clientVersion}
	}
}

func (r *Registry) cleanUp() {
	r.players = make(map[string]PlayerInfo)
	r.nextID = 1
	r.clientVersion = ""
}

func newGuest(r *Registry, prefix string) string {
	for {
		next := rand.Intn(8192)
		name := prefix + strconv.Itoa(next)
		if _, ok := r.players[name]; !ok {
			return name
		}
	}
}
