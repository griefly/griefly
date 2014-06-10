package main

import (
	"fmt"
	"log"
	"strconv"
	"time"
)

const (
	NEXTTICK = 90 * time.Millisecond
)

type Message struct {
	from, to, t string
	content []byte
	id int

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
	id int
	master bool
}

type newPlayerAnswer struct {
	gameId string
	messages chan Message
	mapResp chan Message
	lastID int
}

type LoginEnvelope struct {
	m Message
	response chan clientID
}

type PlayerEnvelope struct {
	id int
	response chan newPlayerAnswer
}

type Registry struct {
	nextId int
	clients []chan Message

	newClients chan LoginEnvelope
	newPlayers chan PlayerEnvelope
	inbox chan Message
	ticker *time.Ticker
}

func newRegistry() *Registry {
	return &Registry{0, nil, make(chan LoginEnvelope),
		make(chan PlayerEnvelope), make(chan Message), nil}
}

// async api
func (r *Registry) AddClient(m Message) (id int, master bool) {
	envelope := LoginEnvelope{m, make(chan clientID, 1)}
	r.newClients <- envelope
	cid := <-envelope.response
	return cid.id, cid.master
}

func (r *Registry) CreatePlayer(id int) (messages, maps chan Message, gameId string, lastID int) {
	pe := PlayerEnvelope{id, make(chan newPlayerAnswer, 1)}
	r.newPlayers <- pe
	response := <-pe.response
	return response.messages, response.mapResp, response.gameId, response.lastID
}

func (r *Registry) CreateMasterPlayer(id int) chan Message {
	pe := PlayerEnvelope{id, make(chan newPlayerAnswer, 1)}
	r.newPlayers <- pe
	response := <-pe.response
	return response.messages
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
	r.clients[0] <-m
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
			if len(newPlayers) > 0 {
				// check if this is a mob creation response
				if m.connId == 0 && m.t == "system" && string(m.content) == "create" {
					reqp, ok := newPlayers[m.from]
					if !ok {
						log.Println("registry: created mob for unknown player, strange. Msg:", m)
					} else {
						// mob for player created on master
						// request map from master
						mapReq := Message{to: m.to, t:"system", content: []byte("need_map")}
						r.sendMaster(mapReq)

						// player now can recieve messages
						// FIXME: make infinity buffer
						r.clients[reqp.id] = make(chan Message, 64)
						log.Println("registry: created channel for player", reqp.id)
						resp := newPlayerAnswer{gameId: m.to, messages: r.clients[reqp.id],
							mapResp: make(chan Message, 1), lastID: lastID}
						// send client a response with his id and map response chan
						reqp.response <- resp
						// move player from newPlayers to mapWaiters
						delete(newPlayers, m.from)
						mapWaiters[m.to] = resp.mapResp
					}
					continue
				}
			}

			if len(mapWaiters) > 0 {
				if m.connId == 0 && m.t == "map" {
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
			reqc.response <- clientID{len(r.clients)-1, len(r.clients)==1}

		case reqp := <-r.newPlayers:
			// add player
			if reqp.id == 0 {
				// this is a master
				messages := make(chan Message, 64)
				r.clients[0] = messages
				log.Println("registry: attached master")
				reqp.response <- newPlayerAnswer{"", messages, nil, 0}
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
		}
	}
}

