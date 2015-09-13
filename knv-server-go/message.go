package main

import (
	"encoding/json"
	"strconv"
)

const (
	MsgidLogin             = 1
	MsgidSuccessfulConnect = 201
	MsgidMapUpload         = 202
	MsgidNewTick           = 203
	MsgidNewClient         = 204

	MsgidWrongGameVersion = 401
	MsgidWrongAuth        = 402
)

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
