package main

import (
	"flag"
	"log"
)


var listen = flag.String("listen", ":1111", "address to listen on")

func main() {
	flag.Parse()

	registry := newRegistry()

	go registry.Run()

	err := ListenAndServe(*listen, registry)
	if err != nil {
		log.Fatal("main: error listening on", listen, err)
	}
}
