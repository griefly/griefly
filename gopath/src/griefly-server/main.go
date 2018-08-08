package main

import (
	"flag"
	"log"
	"math/rand"
	"time"
)

var listen = flag.String("listen", ":1111", "address to listen on")
var metricsListen = flag.String("metrics-listen", "127.0.0.1:9095", "address to listen for prometheus on")
var serverURL = flag.String("server-url", "http://localhost:8011/", "url of asset server")
var tickInterval = flag.Int("tick-interval", 100, "tick interval in ms")
var dumpsRoot = flag.String("dumps-root", "./dumps", "directory to store dumps to")
var dbRoot = flag.String("db-root", "./db", "database root")

func main() {
	flag.Parse()

	rand.Seed(time.Now().Unix())

	TickInterval = time.Duration(*tickInterval) * time.Millisecond

	collector := NewStatsCollector()

	assetServer, err := NewAssetServer(*serverURL, collector)
	if err != nil {
		panic(err)
	}

	db, err := OpenJSONDB(*dbRoot)
	if err != nil {
		panic(err)
	}

	config := &RegistryConfig{DumpRoot: *dumpsRoot}
	registry := newRegistry(assetServer, config, db, collector)

	go registry.Run()

	go assetServer.ListenAndServe()

	go collector.ListenAndServe(*metricsListen)

	err = ListenAndServe(*listen, registry, collector)
	if err != nil {
		log.Fatal("main: error listening on ", *listen, " ", err)
	}
}
