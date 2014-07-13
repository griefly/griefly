package main

import (
	"fmt"
	"log"
	"strconv"
	"time"
)

const (
	NEXTTICK = 100 * time.Millisecond
)

type Message struct {
	from, to, t string
	content     []byte
	id          int

	connId int
}

func (m Message) String() string {
	if len(m.content) <= 64 {
		return fmt.Sprintf("message{id: %d, from: '%s', to: '%s', type: '%s', content: '%s'}",
			m.id, m.from, m.to, m.t, string(m.content))
	} else {
		return fmt.Sprintf("message{id: %d, from: '%s', to: '%s', type: '%s', content: '%s'}",
			m.id, m.from, m.to, m.t, "<skipped>")
	}
}

type clientID struct {
	id     int
	master bool
}

type newPlayerAnswer struct {
	gameId    string
	messages  chan Message
	mapResp   chan Message
	nextInbox chan chan Message
	lastID    int
}

type LoginEnvelope struct {
	m        Message
	response chan clientID
}

type PlayerEnvelope struct {
	id       int
	response chan newPlayerAnswer
}

type Registry struct {
	nextId  int
	clients []chan Message

	masterId int

	newClients chan LoginEnvelope
	newPlayers chan PlayerEnvelope

	oldClients chan int

	inbox  chan Message
	ticker *time.Ticker
}

func newRegistry() *Registry {
	return &Registry{0, nil, -1, make(chan LoginEnvelope),
		make(chan PlayerEnvelope), make(chan int), make(chan Message), nil}
}

// async api
func (r *Registry) AddClient(m Message) (id int, master bool) {
	envelope := LoginEnvelope{m, make(chan clientID, 1)}
	r.newClients <- envelope
	cid := <-envelope.response
	return cid.id, cid.master
}

func (r *Registry) CreatePlayer(id int) (messages, maps chan Message, nextInbox chan chan Message,
	gameId string, lastID int) {

	pe := PlayerEnvelope{id, make(chan newPlayerAnswer, 1)}
	r.newPlayers <- pe
	response := <-pe.response
	return response.messages, response.mapResp, response.nextInbox, response.gameId, response.lastID
}

func (r *Registry) CreateMasterPlayer(id int) chan Message {
	pe := PlayerEnvelope{id, make(chan newPlayerAnswer, 1)}
	r.newPlayers <- pe
	response := <-pe.response
	return response.messages
}

func (r *Registry) RemovePlayer(id int) {
	r.oldClients <- id
}

func (r *Registry) RecvMessage(m Message) {
	r.inbox <- m
}

// internals
func (r *Registry) sendAll(m Message) {
	for _, c := range r.clients {
		if c != nil {
			c <- m
		}
	}
}

func (r *Registry) sendMaster(m Message) {
	if len(r.clients) == 0 {
		log.Fatal("registry: cannot send to master when there are no master")
	}
	r.clients[r.masterId] <- m
}

func (r *Registry) Run() {
	r.ticker = time.NewTicker(NEXTTICK)
	var lastID int
	var newPlayers = make(map[string]PlayerEnvelope)
	var mapWaiters = make(map[string]chan Message)
	for {
		select {
		// new message from client
		case m := <-r.inbox:
			// check if this is a mob creation response
			if m.connId == r.masterId && m.t == "system" && string(m.content) == "create" {
				if len(newPlayers) > 0 {
					reqp, ok := newPlayers[m.from]
					if !ok {
						log.Println("registry: created mob for unknown player, strange. Msg:", m)
					} else {
						// mob for player created on master
						// request map from master
						mapReq := Message{to: m.to, t: "system", content: []byte("need_map")}
						r.sendMaster(mapReq)

						// player now can recieve messages
						r.clients[reqp.id] = make(chan Message, 64)
						tmpMessages := make(chan Message)
						nextInbox := make(chan chan Message)
						// run bufferizator
						go bufferMessages(r.clients[reqp.id], tmpMessages, nextInbox)
						log.Println("registry: created channel for player", reqp.id)
						resp := newPlayerAnswer{gameId: m.to, messages: tmpMessages,
							mapResp: make(chan Message, 1), nextInbox: nextInbox, lastID: lastID}
						// send client a response with his id and map response chan
						reqp.response <- resp
						// move player from newPlayers to mapWaiters
						delete(newPlayers, m.from)
						mapWaiters[m.to] = resp.mapResp
					}
					continue
				}
				continue
			}
			if m.connId == r.masterId && m.t == "map" {
				if len(mapWaiters) > 0 {
					to := m.to
					mapCh, ok := mapWaiters[to]
					if !ok {
						log.Println("registry: got map for unknown player:", to)
					} else {
						mapCh <- m
						delete(mapWaiters, to)
					}
					continue
				}
				continue
			}

			// just resend message
			if m.t == "ordinary" {
				lastID++
				m.id = lastID
			}
			r.sendAll(m)

		case reqc := <-r.newClients:
			// add client
			r.clients = append(r.clients, nil)
			// FIXME: master is just first client
			cID := len(r.clients) - 1
			isMaster := r.masterId == -1
			if isMaster {
				r.masterId = cID
			}
			reqc.response <- clientID{cID, isMaster}

		case reqp := <-r.newPlayers:
			// add player
			if reqp.id == r.masterId {
				// this is a master
				r.masterId = reqp.id
				messages := make(chan Message, 64)
				r.clients[r.masterId] = messages
				log.Println("registry: attached master")
				reqp.response <- newPlayerAnswer{"", messages, nil, nil, lastID}
			} else {
				// this is a slave
				// create a mob for it
				strId := strconv.Itoa(reqp.id)
				lastID++
				createMobMsg := Message{id: lastID, from: strId, t: "ordinary",
					content: []byte("make_new_mob")}
				log.Println("registry: new slave request, creating mob")
				r.sendAll(createMobMsg)
				// add to creation queue
				newPlayers[strId] = reqp
				// then wait for master response...
			}
		case <-r.ticker.C:
			// time for next tick
			lastID++
			nexttick := Message{id: lastID, t: "ordinary", content: []byte("nexttick")}
			r.sendAll(nexttick)
		case old_id := <-r.oldClients:
			s_old_id := strconv.Itoa(old_id)
			if old_id != r.masterId {
				log.Println("registry: detached slave client", old_id)
				r.clients[old_id] = nil
				delete(newPlayers, s_old_id)
				delete(mapWaiters, s_old_id)
			} else {
				log.Println("registry: detached master client", old_id)
				r.masterId = -1
				r.clients[old_id] = nil
				for id, c := range r.clients {
					if c != nil {
						r.masterId = id
						break
					}
				}
				if r.masterId == -1 {
					log.Println("registry: we have no master, we have no world now")
				} else {
					log.Printf("registry: praise the new master %d!", r.masterId)
				}
				delete(newPlayers, s_old_id)
				delete(mapWaiters, s_old_id)

				for _, c := range newPlayers {
					close(c.response)
				}
				for _, c := range mapWaiters {
					close(c)
				}
			}
		}
	}
}
