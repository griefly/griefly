package main

import (
	"encoding/json"
	"io/ioutil"
	"path/filepath"
)

const (
	jsondbAuthFileName = "auth.json"
)

type JSONDB struct {
	auth map[string]*UserInfo
}

func OpenJSONDB(path string) (*JSONDB, error) {
	storage := &JSONDB{
		auth: make(map[string]*UserInfo),
	}

	buf, err := ioutil.ReadFile(filepath.Join(path, jsondbAuthFileName))
	if err != nil {
		return nil, err
	}

	err = json.Unmarshal(buf, &storage.auth)
	if err != nil {
		return nil, err
	}

	for login, info := range storage.auth {
		info.Login = login
	}

	return storage, nil
}

func (db *JSONDB) GetUserInfo(username string) (*UserInfo, error) {
	info := db.auth[username]
	return info, nil
}
