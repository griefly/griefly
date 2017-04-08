package main

import (
	"bufio"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net"
	"strconv"
	"strings"

	"gopkg.in/validator.v2"
)

const (
	MaxMessageLength  = 1 * 1024 * 1024 // 1M
	ProtoVersionMagic = "S132"
)

type MessageIsTooLongError int

func (m MessageIsTooLongError) Error() string {
	return fmt.Sprintf("message is too long: %d", int(m))
}

type ClientConnection struct {
	conn     net.Conn
	bufConn  *bufio.ReadWriter
	reg      *Registry
	inbox    chan *Envelope
	readErrs chan error

	id int

	collector *StatsCollector
}

func (c *ClientConnection) readMessage() (*Envelope, error) {
	// read header
	header := make([]byte, 8)
	_, err := io.ReadFull(c.bufConn, header)
	if err != nil {
		return nil, err
	}

	length := int(binary.BigEndian.Uint32(header[:4]))
	kind := binary.BigEndian.Uint32(header[4:])

	maxLen, ok := maxMessageLength[kind]
	if !ok {
		maxLen = MaxMessageLength
	}

	if length > maxLen {
		return nil, MessageIsTooLongError(length)
	}

	if length == 0 {
		return NewEnvelope(emptyMessage, kind, 0), nil
	}

	// read body
	buf := make([]byte, length)
	_, err = io.ReadFull(c.bufConn, buf)

	if err != nil {
		return nil, err
	}

	msg := getConcreteMessage(kind)
	err = json.Unmarshal(buf, msg)
	if err != nil {
		log.Println("failed to unmarshal message:", string(buf), err)
		return nil, err
	}

	// validate payload
	err = validator.Validate(msg)
	if err != nil && err != validator.ErrUnsupported {
		log.Println("failed to validate message:", string(buf), err)
		return nil, err
	}

	return NewEnvelope(msg, kind, 0), nil
}

func (c *ClientConnection) writeMessage(e *Envelope) error {
	var buf []byte
	var err error

	c.collector.ObserveOutgoingMessage(e)

	if e.Message != emptyMessage {
		buf, err = json.Marshal(e.Message)
		if err != nil {
			return err
		}
	}

	length := uint32(len(buf))
	var header [8]byte
	binary.BigEndian.PutUint32(header[:4], length)
	binary.BigEndian.PutUint32(header[4:], e.Kind)

	_, err = c.bufConn.Write(header[:])
	if err != nil {
		return err
	}

	_, err = c.bufConn.Write(buf)
	if err != nil {
		return err
	}

	err = c.bufConn.Writer.Flush()
	c.collector.ObserveSentMessage(e)
	return err
}

func (c *ClientConnection) reciever() {
	for {
		e, err := c.readMessage()
		if err != nil {
			c.readErrs <- err
			return
		}

		e.From = c.id
		c.collector.ObserveIncomingMessage(e)
		c.reg.RecvMessage(e)
	}
}

type version struct {
	numParts []int
	rest     string
	orig     string
}

func parseVersion(ver string) version {
	ver = strings.TrimLeft(ver, "v")
	var parts []string
	outer := strings.Split(ver, "-")
	inner := strings.Split(outer[0], ".")

	parts = append(parts, inner...)
	if len(outer) > 1 {
		parts = append(parts, outer[1])
	}

	var rest string
	if len(outer) > 2 {
		rest = strings.Join(outer[2:], "-")
	}

	var numParts []int
	for _, p := range parts {
		num, _ := strconv.Atoi(p) // ignore error, assume zero
		numParts = append(numParts, num)
	}

	return version{numParts, rest, ver}
}

func (v version) String() string {
	return v.orig
}

func (v version) less(other version) bool {
	for i, num := range other.numParts {
		if i >= len(v.numParts) {
			return true
		}

		if v.numParts[i] < num {
			return true
		}

		if v.numParts[i] > num {
			return false
		}
	}

	return false
}

var clientVersionBuild string

var clientVersion = parseVersion(clientVersionBuild)

func (c *ClientConnection) Run() {
	defer c.conn.Close()

	var err error

	var version = make([]byte, 4)
	_, err = c.bufConn.Read(version)
	if err != nil {
		log.Println("client: error on reading protocol version:", err)
		return
	}

	if string(version) != ProtoVersionMagic {
		log.Println("client: wrong protocol version:", string(version))
		return
	}

	// read version
	e, err := c.readMessage()
	if err != nil {
		log.Println("client: failed to read initial login message:", err)
		return
	}

	if e.Kind != MsgidLogin {
		log.Println("client: invalid first message kind:", e.Kind)
	}

	login := e.Message.(*MessageLogin)

	if parseVersion(login.GameVersion).less(clientVersion) {
		log.Printf("client is too old. Server version: '%s', client version: '%s'",
			clientVersionBuild, login.GameVersion)
		msg := &ErrmsgWrongGameVersion{CorrectVersion: clientVersionBuild}
		reply := NewEnvelope(msg, MsgidWrongGameVersion, 0)
		c.writeMessage(reply)
		return
	}

	ok, currentVersion := c.reg.CheckVersion(login.GameVersion)
	if !ok {
		log.Printf("client version mismatch. Game version: '%s', client version: '%s'",
			currentVersion, login.GameVersion)
		msg := &ErrmsgWrongGameVersion{CorrectVersion: currentVersion}
		reply := NewEnvelope(msg, MsgidWrongGameVersion, 0)
		c.writeMessage(reply)
		return
	}

	log.Println("client: registering client")
	var mapDownloadURL string
	var master bool
	var errMsg *Envelope
	c.inbox, c.id, master, mapDownloadURL, errMsg = c.reg.CreatePlayer(e)

	if errMsg != nil {
		// registry refused to register player
		c.writeMessage(errMsg)
		return
	}

	log.Printf("client[%d]: registered, is it master? %t", c.id, master)

	if master {
		msg := &MessageSuccessfulConnect{MapURL: "no_map", ID: &c.id}
		e := NewEnvelope(msg, MsgidSuccessfulConnect, 0)
		err = c.writeMessage(e)

		if err != nil {
			c.reg.RemovePlayer(c.id, nil)
			log.Printf("client[%d]: failed to send hello response: %v", c.id, err)
			return
		}
	} else {
		msg := &MessageSuccessfulConnect{MapURL: mapDownloadURL, ID: &c.id}
		e := NewEnvelope(msg, MsgidSuccessfulConnect, 0)
		err = c.writeMessage(e)

		if err != nil {
			c.reg.RemovePlayer(c.id, nil)
			log.Printf("client[%d]: failed to send hello response: %v", c.id, err)
			return
		}
	}

	// login complete, run game loop
	log.Printf("client[%d]: starting main loop", c.id)
	go c.reciever()
	for {
		select {
		case m, ok := <-c.inbox:
			if !ok {
				// channel is empty, close connection
				log.Printf("client[%d]: inbox closed, closing connection", c.id)
				c.conn.Close()
				return
			}
			err := c.writeMessage(m)
			if err != nil {
				go c.reg.RemovePlayer(c.id, nil)
				log.Printf("client[%d]: sender failed to send message: %s", c.id, err.Error())

				// blackhole all writes for this client to prevent registry deadlock
				for {
					_, ok := <-c.inbox
					if !ok {
						break
					}
				}
				return
			}
		case err = <-c.readErrs:
			c.reg.RemovePlayer(c.id, nil)
			log.Printf("client[%d]: reciever failed to read message: %s", c.id, err.Error())
			return
		}
	}
}

func ListenAndServe(addr string, r *Registry, collector *StatsCollector) error {
	socket, err := net.Listen("tcp", addr)
	if err != nil {
		return err
	}

	for {
		conn, err := socket.Accept()
		if err != nil {
			log.Println("listener: error on accept:", err)
			continue
		}

		bufrw := bufio.NewReadWriter(bufio.NewReader(conn), bufio.NewWriter(conn))
		client := &ClientConnection{
			conn:      conn,
			bufConn:   bufrw,
			reg:       r,
			id:        -1,
			readErrs:  make(chan error, 1),
			collector: collector,
		}
		go client.Run()
	}
}
