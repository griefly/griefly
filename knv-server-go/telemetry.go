package main

import (
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

type DumpWriter struct {
	root      string
	sessionID string
}

func NewDumpWriter(root, sessionID string) *DumpWriter {
	return &DumpWriter{root, sessionID}
}

func (dw *DumpWriter) DumpMap(tag string, source *Pipe, callback func()) {
	defer func() {
		if err := recover(); err != nil {
			log.Printf("dump-writer: catched panic during map dump: %v", err)
		}
	}()

	// TODO: timeouts
	info := <-source.info
	if info.Err != nil {
		log.Printf("dump-writer: client failed to upload map", info.Err)
	}

	// in case of error pump rest of request body to bitsink
	defer io.Copy(ioutil.Discard, source)

	dir := filepath.Join(dw.root, dw.sessionID)
	os.MkdirAll(dir, os.ModePerm)
	filename := tag + ".mapdump"

	if info.ContentType == QZippedContentType {
		filename += ".qz"
	}

	target, err := os.Create(filepath.Join(dir, filename))
	if err != nil {
		log.Printf("dump-writer: failed to create dump file: %v", err)
		return
	}

	defer func() {
		target.Close()
		if err != nil {
			os.Remove(filename)
		}
	}()

	_, err = io.Copy(target, source)
	if err != nil {
		log.Printf("dump-writer: failed to write dump: %v", err)
	}

	// everything is ok, notify customer
	if callback != nil {
		callback()
	}
}
