package main

import (
	"flag"
	"log"
	"net/http"
	"net/url"
)

var listen = flag.String("listen", ":1111", "address to listen on")
var serverURL = flag.String("server-url", "http://localhost/", "url of asset server")

func main() {
	flag.Parse()

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
