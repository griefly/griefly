package main

type UserInfo struct {
	Login    string `json:"login"`
	Passhash string `json:"passhash"`
	IsAdmin  bool   `json:"is-admin"`
}

type DB interface {
	GetUserInfo(login string) (*UserInfo, error)
}
