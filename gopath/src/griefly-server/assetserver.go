package main

import (
	"bytes"
	"errors"
	"io"
	"math/rand"
	"net/http"
	"net/url"
	"strconv"
	"strings"
	"sync"
	"time"
)

const (
	PipeQueueLength    = 4
	DefalutContentType = "application/octet-stream"
	QZippedContentType = "application/zip"
)

type AssetServer struct {
	serverURL *url.URL

	pipes     map[string]*Pipe
	pipeMutex sync.Mutex

	collector *StatsCollector
}

func NewAssetServer(serverURL string, collector *StatsCollector) (*AssetServer, error) {
	u, err := url.Parse(serverURL)
	if err != nil {
		return nil, err
	}

	return &AssetServer{u, make(map[string]*Pipe), sync.Mutex{}, collector}, nil
}

func (as *AssetServer) ListenAndServe() error {
	var port string
	colonIdx := strings.Index(as.serverURL.Host, ":")
	if colonIdx < 0 {
		port = ":80"
	} else {
		port = as.serverURL.Host[colonIdx:]
	}

	server := http.Server{Addr: port, Handler: as}
	return server.ListenAndServe()
}

func (as *AssetServer) MakePipe() (*Pipe, string, string) {
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

		if readID == "" {
			readID = id
		} else if writeID == "" {
			writeID = id
		} else {
			break
		}
	}

	pipe := NewPipe(readID, writeID)
	as.pipes[readID] = pipe
	as.pipes[writeID] = pipe
	return pipe, as.makePipeURL(writeID), as.makePipeURL(readID)
}

func (as *AssetServer) ServeHTTP(rw http.ResponseWriter, req *http.Request) {
	defer req.Body.Close()
	id := req.URL.Query().Get("id")

	if id == "" {
		http.Error(rw, "", http.StatusForbidden)
		return
	}

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
	surl := *as.serverURL
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

	contentType := info.ContentType
	if contentType == "" {
		contentType = DefalutContentType
	}

	rw.Header().Set("Content-Length", strconv.Itoa(int(info.Size)))
	rw.Header().Set("Content-Type", contentType)

	start := time.Now()
	_, err := io.Copy(rw, pipe)
	dur := time.Since(start)
	if err != nil {
		// TODO(mechmind): handle error
	}

	as.collector.ObservePipeDownloadTime(dur)
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
	info.ContentType = req.Header.Get("Content-Type")

	as.collector.ObservePipeLength(int64(length))
	pipe.info <- info

	_, err = io.Copy(pipe, io.LimitReader(req.Body, int64(length)))
	if err != nil {
		info.Err = err
		pipe.info <- info
	}
	pipe.Close()
}

type PipeInfo struct {
	Size        int64
	Err         error
	ContentType string
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
