package main

import (
	"bytes"
	"errors"
	"io"
	"math/rand"
	"net/http"
	"net/url"
	"strconv"
	"sync"
)

const (
	PipeQueueLength = 4
)

type AssetServer struct {
	serverURL string

	pipes     map[string]*Pipe
	pipeMutex sync.Mutex
}

func NewAssetServer(serverURL string) (*AssetServer, error) {
	_, err := url.Parse(serverURL)
	if err != nil {
		return nil, err
	}

	return &AssetServer{serverURL, make(map[string]*Pipe), sync.Mutex{}}, nil
}

func (as *AssetServer) MakePipe() (input, output string) {
	as.pipeMutex.Lock()
	defer as.pipeMutex.Unlock()

	// make unique ids
	var readID, writeID string
	for {
		randNumber := rand.Int63()
		id := strconv.Itoa(int(randNumber))
		if _, ok := as.pipes[id]; ok {
			continue
		}

		switch {
		case readID == "":
			readID = id
		case writeID == "":
			writeID = id
		default:
			break
		}
	}

	pipe := NewPipe(readID, writeID)
	as.pipes[readID] = pipe
	as.pipes[writeID] = pipe
	return as.makePipeURL(readID), as.makePipeURL(writeID)
}

func (as *AssetServer) ServeHTTP(rw http.ResponseWriter, req *http.Request) {
	defer req.Body.Close()
	id := req.URL.Query().Get("id")

	as.pipeMutex.Lock()
	pipe, ok := as.pipes[id]
	if ok {
		delete(as.pipes, id)
	}
	as.pipeMutex.Unlock()

	if !ok {
		http.Error(rw, "no such id", http.StatusNotFound)
		return
	}

	if req.Method == "GET" {
		// download
		as.pipeDownload(rw, req, id, pipe)
	} else if req.Method == "POST" {
		// upload
		as.pipeUpload(rw, req, id, pipe)

	} else {
		http.Error(rw, "unsupported", http.StatusMethodNotAllowed)
	}
}

func (as *AssetServer) makePipeURL(id string) string {
	surl, err := url.Parse(as.serverURL)
	if err != nil {
		// must be validated already
		panic(err)
	}

	query := surl.Query()
	query.Set("id", id)
	surl.RawQuery = query.Encode()
	return surl.String()
}

func (as *AssetServer) pipeDownload(rw http.ResponseWriter, req *http.Request, id string, pipe *Pipe) {
	if id != pipe.in {
		http.Error(rw, "invalid request", http.StatusBadRequest)
		return
	}

	info := <-pipe.info
	if info.Err != nil {
		http.Error(rw, "we failed", http.StatusInternalServerError)
		return
	}

	rw.Header().Set("Content-Length", strconv.Itoa(int(info.Size)))
	rw.Header().Set("Content-Type", "application/octet-stream")

	_, err := io.Copy(rw, pipe)
	if err != nil {
		// TODO(mechmind): handle error
	}

}

func (as *AssetServer) pipeUpload(rw http.ResponseWriter, req *http.Request, id string, pipe *Pipe) {
	info := &PipeInfo{}
	if id != pipe.out {
		info.Err = errors.New("uploader is dead")
		pipe.info <- info
		http.Error(rw, "invalid request", http.StatusBadRequest)
		return
	}

	length, err := strconv.Atoi(req.Header.Get("Content-Length"))
	if err != nil {
		info.Err = errors.New("uploader is dead")
		pipe.info <- info
		http.Error(rw, "invalid content length", http.StatusBadRequest)
		return
	}

	info.Size = int64(length)
	pipe.info <- info

	_, err = io.Copy(pipe, io.LimitReader(req.Body, int64(length)))
	if err != nil {
		info.Err = err
		pipe.info <- info
	}
}

type PipeInfo struct {
	Size int64
	Err  error
}

type Pipe struct {
	in, out string
	readBuf *bytes.Buffer
	readErr error
	data    chan []byte
	info    chan *PipeInfo
}

func NewPipe(in, out string) *Pipe {
	return &Pipe{
		in:      in,
		out:     out,
		readBuf: &bytes.Buffer{},
		data:    make(chan []byte, PipeQueueLength),
		info:    make(chan *PipeInfo, 1),
	}
}

func (p *Pipe) Write(buf []byte) (int, error) {
	transit := make([]byte, len(buf))
	copy(transit, buf)
	p.data <- transit
	return len(buf), nil
}

func (p *Pipe) Read(buf []byte) (int, error) {
	if p.readErr != nil {
		return 0, p.readErr
	}

	if p.readBuf.Len() > 0 {
		return p.readBuf.Read(buf)
	}

	var next []byte
	var ok bool

	select {
	case next, ok = <-p.data:
		if !ok {
			return 0, io.EOF
		}
	case info := <-p.info:
		if info.Err != nil {
			p.readErr = info.Err
			return 0, info.Err
		}
	}

	n := copy(buf, next)
	if n < len(next) {
		p.readBuf.Write(next[n:])
	}

	return n, nil
}

func (p *Pipe) Close() error {
	close(p.data)
	return nil
}
