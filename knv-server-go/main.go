package main

import (
	"flag"
	"log"
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

	err = ListenAndServe(*listen, registry)
	if err != nil {
		log.Fatal("main: error listening on", listen, err)
	}
}
