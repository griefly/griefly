package main

import (
	"bytes"
	"io"
	"log"
	"net"
	"strconv"
)

type ClientConnection struct {
	conn     net.Conn
	reg      *Registry
	inbox    chan Message
	readErrs chan error

	id     int
	master bool
	gameId string
}

func (c *ClientConnection) readMessage() (Message, error) {
	lengthStr, err := readUntil(c.conn, ' ')
	if err != nil {
		return Message{}, err
	}

	length, err := strconv.Atoi(string(lengthStr))
	if err != nil {
		return Message{}, err
	}

	// TODO: buffer cache
	data := make([]byte, 0, length)
	buf := bytes.NewBuffer(data)

	_, err = io.CopyN(buf, c.conn, int64(length))
	if err != nil {
		return Message{}, err
	}

	data = buf.Bytes()

	var m Message
	// read message fields
	var n int
	// read msg number
	n = bytes.IndexByte(data, ' ')
	m.id, _ = strconv.Atoi(string(data[:n]))
	data = data[n+1:]

	// read from
	n = bytes.IndexByte(data, ' ')
	m.from = string(data[:n])
	data = data[n+1:]

	// read to
	n = bytes.IndexByte(data, ' ')
	m.to = string(data[:n])
	data = data[n+1:]

	// read type
	n = bytes.IndexByte(data, ' ')
	m.t = string(data[:n])
	data = data[n+1:]

	// set content
	m.content = data

	if m.t == "system" && string(m.content) == "hash" {
	} else {
		//log.Printf("client[%d]: recieved message: %s", c.id, m.String())
	}
	return m, nil
}

func (c *ClientConnection) writeMessage(m Message) error {

	if m.from == "" {
		m.from = "-1"
	}
	if m.to == "" {
		m.to = "-1"
	}
	if m.t == "" {
		m.t = "-1"
	}
	if len(m.content) == 0 {
		m.content = []byte("-1")
	}

	if m.t == "system" && string(m.content) == "hash" {
	} else if m.t == "ordinary" && string(m.content) == "nexttick" {
	} else {
		//log.Printf("client[%d]: sending message: %s", c.id, m.String())
	}

	// 4 = len of four spaces inbetween
	mid := strconv.Itoa(m.id)
	headerLen := len(mid) + len(m.from) + len(m.to) + len(m.t) + len(m.content) + 4
	header := strconv.Itoa(headerLen) + " " + mid + " " + m.from + " " + m.to + " " + m.t + " "

	// write header
	_, err := c.conn.Write([]byte(header))
	if err != nil {
		return err
	}

	// write content
	_, err = io.Copy(c.conn, bytes.NewBuffer(m.content))
	if err != nil {
		return err
	}

	return nil
}

func (c *ClientConnection) reciever() {
	for {
		m, err := c.readMessage()
		if err != nil {
			c.readErrs <- err
			return
		}

		m.connId = c.id
		if m.t == "ordinary" || m.t == "chat" {
			m.to = c.gameId
		}
		c.reg.RecvMessage(m)
	}
}

func (c *ClientConnection) Run() {
	defer c.conn.Close()

	// read hello
	m, err := c.readMessage()
	if err != nil {
		log.Println("client: failed to read 'hello' message:", err)
		return
	}

	log.Println("client: recieved hello")

	c.gameId = m.from
	log.Println("client: registering client")
	c.id, c.master = c.reg.AddClient(m)
	log.Println("client: registered as", c.id, "master?", c.master)
	var nextInboxChan chan chan Message
	if c.master {
		// just respond with almost same message
		// that client have reference map
		log.Println("client: creating master...")
		c.inbox = c.reg.CreateMasterPlayer(c.id)
		log.Println("client: created master")
		m.content = []byte("nomap")
		err = c.writeMessage(m)
		if err != nil {
			c.reg.RemovePlayer(c.id)
			log.Println("client: failed to send hello response:", err)
			return
		}
	} else {
		// create new player
		var mapChan chan Message
		var lastID int
		c.inbox, mapChan, nextInboxChan, c.gameId, lastID = c.reg.CreatePlayer(c.id)
		// send 'map' message to client
		waitMapM := Message{from: strconv.Itoa(lastID), to: c.gameId, t: "system",
			content: []byte("map")}
		err = c.writeMessage(waitMapM)
		if err != nil {
			c.reg.RemovePlayer(c.id)
			log.Printf("client[%d]: failed to send waitmap message: %s", c.id, err.Error())
			return
		}
		// wait for map from master
		mapM := <-mapChan
		// send map to client
		err = c.writeMessage(mapM)
		if err != nil {
			c.reg.RemovePlayer(c.id)
			log.Println("client[%d]: failed to send map message: %s", c.id, err.Error())
			return
		}
	}

	// login complete

	var nextInbox chan Message
	// run game loop
	log.Printf("client[%d]: starting main loop", c.id)
	go c.reciever()
	for {
		select {
		case m, ok := <-c.inbox:
			if !ok {
				// channel is empty, switch to next
				log.Printf("client[%d]: switching channels...", c.id)
				c.inbox = nextInbox
			} else {
				err := c.writeMessage(m)
				if err != nil {
					c.reg.RemovePlayer(c.id)
					log.Printf("client[%d]: sender failed to send message: %s", c.id, err.Error())
					return
				}
			}
		case err = <-c.readErrs:
			c.reg.RemovePlayer(c.id)
			log.Printf("client[%d]: reciever failed to read message: %s", c.id, err.Error())
			return
		case nextInbox = <-nextInboxChan:
		}
	}
}

func ListenAndServe(addr string, r *Registry) error {
	socket, err := net.Listen("tcp", addr)
	if err != nil {
		return err
	}
    
    var version = make([]byte, 4)
    
	for {
		conn, err := socket.Accept()
		if err != nil {
			log.Println("listener: error on accept:", err)
			continue
		}
        
         _, err = conn.Read(version)
        if err != nil {
			log.Println("listener: error on accept:", err)
			continue
        }
        
        var str_version = string(version[:])
        
        if str_version != "S131" {
            log.Println("listener: wrong protocol version:", str_version)
            conn.Close()
            continue
        }

		client := &ClientConnection{conn: conn, reg: r, id: -1}
		go client.Run()
	}
}
