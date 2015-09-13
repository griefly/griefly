package main

import (
	"bufio"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io"
	"math/rand"
	"net"
	"strconv"
	"time"
)

const (
	ProtoHeader = "S132"
	Remote      = "localhost:1111"
)

type Value map[string]interface{}

func writeMessage(w io.Writer, kind int, msg Value) error {
	data, err := json.Marshal(msg)
	if err != nil {
		return err
	}

	length := len(data)
	header := make([]byte, 8)

	binary.BigEndian.PutUint32(header[:4], uint32(length))
	binary.BigEndian.PutUint32(header[4:], uint32(kind))

	_, err = w.Write(header)
	if err != nil {
		return err
	}

	_, err = w.Write(data)

	return err
}

func readMessage(r io.Reader) (kind int, msg Value, err error) {
	header := make([]byte, 8)
	_, err = r.Read(header)
	if err != nil {
		return
	}

	length := binary.BigEndian.Uint32(header[:4])
	kind = int(binary.BigEndian.Uint32(header[4:]))
	buf := make([]byte, int(length))
	_, err = r.Read(buf)
	if err != nil {
		return
	}

	err = json.Unmarshal(buf, &msg)
	return
}

func makeRandomMessage(counter int) Value {
	return Value{
		"counter": counter,
	}
}

func main() {
	rand.Seed(time.Now().Unix())

	conn, err := net.Dial("tcp", Remote)
	if err != nil {
		panic(err)
	}
	defer conn.Close()

	remote := bufio.NewReadWriter(bufio.NewReader(conn), bufio.NewWriter(conn))

	remote.Write([]byte(ProtoHeader))

	// login
	login := "me-" + strconv.Itoa(rand.Intn(4096))
	password := "letmein"

	msg := Value{"login": login, "password": password}
	writeMessage(remote, 1, msg)
	remote.Flush()

	var counter int
	for {
		kind, msg, err := readMessage(remote)
		if err != nil {
			fmt.Println("failed to read message:", err)
			break
		}
		fmt.Printf("msg of %d: %v\n", kind, msg)
		if counter%10 == 4 {
			fmt.Println("sending random message")
			writeMessage(remote, 1001, makeRandomMessage(counter))
			remote.Flush()
		}
		counter++
	}
}
