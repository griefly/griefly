package main

import (
	"flag"
	"log"
	"math/rand"
	"net/http"
	"net/url"
	"time"
)

var listen = flag.String("listen", ":1111", "address to listen on")
var serverURL = flag.String("server-url", "http://localhost:8011/", "url of asset server")
var tickInterval = flag.Int("tick-interval", 100, "tick interval in ms")

func main() {
	flag.Parse()

	rand.Seed(time.Now().Unix())

	TickInterval = time.Duration(*tickInterval) * time.Millisecond

	assetServer, err := NewAssetServer(*serverURL)
	if err != nil {
		panic(err)
	}

	registry := newRegistry(assetServer)

	go registry.Run()

	u, _ := url.Parse(*serverURL)
	go http.ListenAndServe(u.Host, assetServer)

	err = ListenAndServe(*listen, registry)
	if err != nil {
		log.Fatal("main: error listening on", listen, err)
	}
}
