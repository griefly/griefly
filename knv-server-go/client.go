package main

import (
	"log"
	"time"
)

const (
	NEXTTICK          = 100 * time.Millisecond
	PlayerQueueLength = 32
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

type Registry struct {
	nextID  int
	clients map[int]chan *Envelope
	players map[string]PlayerInfo

	masterID int

	newPlayers chan PlayerEnvelope

	droppedPlayers chan int

	inbox       chan *Envelope
	ticker      *time.Ticker
	assetServer *AssetServer
}

func newRegistry(as *AssetServer) *Registry {
	return &Registry{0, make(map[int]chan *Envelope), make(map[string]PlayerInfo), -1,
		make(chan PlayerEnvelope), make(chan int), make(chan *Envelope), nil, as}
}

// async api
func (r *Registry) CreatePlayer(m *Envelope) (inbox chan *Envelope, id int, master bool, mapURL string) {
	pe := PlayerEnvelope{m, make(chan newPlayerReply, 1)}
	r.newPlayers <- pe
	response := <-pe.response
	return response.inbox, response.id, response.master, response.mapDownloadURL
}

func (r *Registry) RemovePlayer(id int) {
	r.droppedPlayers <- id
}

func (r *Registry) RecvMessage(e *Envelope) {
	r.inbox <- e
}

// internals
func (r *Registry) sendAll(m *Envelope) {
	var deadPlayers []int
	for idx, c := range r.clients {
		if c != nil {
			select {
			case c <- m:
			default:
				// client is too slow, drop him
				deadPlayers = append(deadPlayers, idx)
			}
		}
	}

	for _, idx := range deadPlayers {
		r.removePlayer(idx)
	}
}

func (r *Registry) sendMaster(m *Envelope) {
	if len(r.clients) == 0 {
		log.Fatal("registry: cannot send to master when there are no clients at all")
	}
	r.clients[r.masterID] <- m
}

func (r *Registry) Run() {
	r.ticker = time.NewTicker(NEXTTICK)

	for {
		select {
		case m := <-r.inbox:
			r.sendAll(m)
		case <-r.ticker.C:
			m := &MessageNewTick{}
			e := &Envelope{m, MsgidNewTick}
			r.sendAll(e)
		case newPlayer := <-r.newPlayers:
			r.registerPlayer(newPlayer)
		case playerID := <-r.droppedPlayers:
			r.removePlayer(playerID)
		}
	}
}

func (r *Registry) registerPlayer(newPlayer PlayerEnvelope) {
	m := newPlayer.m.Message.(*MessageLogin)
	var id int
	var existed bool
	// look up for existing player avatar for current map
	if info, ok := r.players[m.Login]; ok {
		id = info.id
		existed = true
		log.Printf("registry: reusing existing id %d for player '%s'", id, m.Login)
	} else {
		id = r.nextID
		r.nextID++
		info = PlayerInfo{id: id, login: m.Login}
		r.players[m.Login] = info
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
		mapUploadURL, mapDownloadURL = r.assetServer.MakePipe()
		m := &MessageMapUpload{mapUploadURL}
		e := &Envelope{m, MsgidMapUpload}
		r.sendMaster(e)
	}

	if !existed {
		// create player on maps
		newm := &MessageNewClient{id}
		e := &Envelope{newm, MsgidNewClient}
		r.sendAll(e)
		log.Printf("registry: creating new unit for client %d, login '%s'", id, m.Login)
	}

	response := newPlayerReply{id: id, master: master, mapDownloadURL: mapDownloadURL, inbox: inbox}
	newPlayer.response <- response
}

func (r *Registry) removePlayer(id int) {
	inbox, ok := r.clients[id]
	if !ok {
		// already deleted
		return
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

func (r *Registry) cleanUp() {
	r.players = make(map[string]PlayerInfo)
	r.nextID = 0
}
