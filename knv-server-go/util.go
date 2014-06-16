package main

import (
	"io"
)

func readUntil(src io.Reader, needle byte) ([]byte, error) {
	var buf = make([]byte, 0, 64)
	var one = make([]byte, 1)

	for {
		_, err := src.Read(one)
		if err != nil {
			return buf, err
		}

		if one[0] == needle {
			return buf, nil
		}

		buf = append(buf, one...)
	}
}
