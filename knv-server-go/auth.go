package main

import "errors"

var (
	GuestInfo = &UserInfo{}
)

var (
	ErrNotAuthenticated = errors.New("not authenticated")
)

func Authenticate(db DB, username, passhash string, isGuest bool) (*UserInfo, error) {
	if isGuest {
		return &UserInfo{}, nil
	}

	info, err := db.GetUserInfo(username)
	if err != nil {
		return nil, err
	}

	if info == nil {
		return nil, ErrNotAuthenticated
	}

	if info.Passhash != passhash {
		return nil, ErrNotAuthenticated
	}

	return info, nil
}
