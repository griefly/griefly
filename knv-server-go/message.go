package main

import (
	"encoding/json"
	"strconv"
)

const (
	MsgidLogin              = 1
	MsgidExit               = 2
	MsgidHash               = 3
	MsgidRestart            = 4
	MsgidSuccessfulConnect  = 201
	MsgidMapUpload          = 202
	MsgidNewTick            = 203
	MsgidNewClient          = 204
	MsgidCurrentConnections = 205

	MsgidWrongGameVersion = 401
	MsgidWrongAuth        = 402

	MsgidOrdinary    = 1001
	MsgidJustMessage = 1002
	MsgidGUI         = 1003
	MsgidMouseClick  = 1004
	MsgidOOCMessage  = 1005
	MsgidPing        = 1102
)

var maxMessageLength = map[uint32]int{
	MsgidLogin: 512,
	MsgidHash:  512,

	MsgidWrongGameVersion: 512,

	MsgidSuccessfulConnect:  512,
	MsgidMapUpload:          512,
	MsgidNewClient:          128,
	MsgidCurrentConnections: 128,

	MsgidOrdinary:    512,
	MsgidJustMessage: 2048,
	MsgidMouseClick:  512,
	MsgidOOCMessage:  2048,
	MsgidPing:        512,
}

type Message interface {
	TypeName() string
}

type Forwardable interface {
	SetID(int)
}

type Envelope struct {
	Message
	Kind uint32
	From int
}

var (
	emptyMessage = &MessageEmpty{}
)

func getConcreteMessage(id uint32) Message {
	switch {
	case id == MsgidLogin:
		return &MessageLogin{}
		/*
			case id == 2:
				return &MessageInput{}
			case id == 3:
				return &MessageChat{}
		*/

	case id == MsgidSuccessfulConnect:
		return &MessageSuccessfulConnect{}
	case id == MsgidMapUpload:
		return &MessageMapUpload{}
	case id == MsgidNewTick:
		return &MessageNewTick{}
	case id == MsgidNewClient:
		return &MessageNewClient{}

		// errors
	case id == MsgidWrongGameVersion:
		return &ErrmsgWrongGameVersion{}
	case id == MsgidWrongAuth:
		return &ErrmsgWrongAuth{}
	case id >= 1000:
		m := OpaqueMessage(make(map[string]*json.RawMessage))
		return &m
	}
	return nil
}

type MessageIDEmbed struct {
	ID int `json:"id"`
}

func (m *MessageIDEmbed) SetID(id int) {
	m.ID = id
}

type MessageEmpty struct{}

func (m *MessageEmpty) TypeName() string {
	return "<empty>"
}

type MessageInput struct {
	MessageIDEmbed
	Key string `json:"key"`
}

func (m *MessageInput) TypeName() string {
	return "MessageInput"
}

type MessageChat struct {
	MessageIDEmbed
	Type string `json:"type"`
	Text string `json:"text"`
}

func (m *MessageChat) TypeName() string {
	return "MessageChat"
}

type MessageLogin struct {
	Login       string `json:"login"`
	Password    string `json:"password"`
	IsGuest     bool   `json:"guest"`
	GameVersion string `json:"game_version"`
}

func (m *MessageLogin) TypeName() string {
	return "MessageLogin"
}

type MessageSuccessfulConnect struct {
	ID     int    `json:"your_id"`
	MapURL string `json:"map"`
}

func (m *MessageSuccessfulConnect) TypeName() string {
	return "MessageSuccessfulConnect"
}

type MessageMapUpload struct {
	MapURL string `json:"url_to_upload_map"`
}

func (m *MessageMapUpload) TypeName() string {
	return "MessageMapUpload"
}

type MessageNewTick struct{}

func (m *MessageNewTick) TypeName() string {
	return "MessageNewTick"
}

type MessageNewClient struct {
	ID int `json:"id"`
}

func (m *MessageNewClient) TypeName() string {
	return "MessageNewClient"
}

// errors
type ErrmsgWrongGameVersion struct {
	CorrectVersion string `json:"correct_game_version"`
}

func (m *ErrmsgWrongGameVersion) TypeName() string {
	return "ErrmsgWrongGameVersion"
}

type ErrmsgWrongAuth struct {
}

func (m *ErrmsgWrongAuth) TypeName() string {
	return "ErrmsgWrongAuth"
}

type OpaqueMessage map[string]*json.RawMessage

func (m *OpaqueMessage) TypeName() string {
	return "OpaqueMessage"
}

func (m *OpaqueMessage) SetID(id int) {
	bytesID := json.RawMessage(strconv.Itoa(id))
	(*m)["id"] = &bytesID
}
