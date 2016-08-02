package main

import (
	"github.com/prometheus/client_golang/prometheus"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"time"
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

type StatsCollector struct {
	// session counters
	CurrentClients      prometheus.Gauge
	TotalSessionClients prometheus.Counter

	// message metrics
	MessageDeliveryTime *prometheus.SummaryVec
	MessageCounter      *prometheus.CounterVec

	// asset server metrics
	PipeBodyLength   prometheus.Summary
	PipeDownloadTime prometheus.Summary
}

func NewStatsCollector() *StatsCollector {
	collector := &StatsCollector{}
	collector.CurrentClients = prometheus.NewGauge(prometheus.GaugeOpts{
		Name: "current_clients",
		Help: "Number of currently connected clients",
	})
	prometheus.MustRegister(collector.CurrentClients)

	collector.TotalSessionClients = prometheus.NewCounter(prometheus.CounterOpts{
		Name: "total_session_clients",
		Help: "Total number of clients ever registered in current session",
	})
	prometheus.MustRegister(collector.TotalSessionClients)

	collector.MessageDeliveryTime = prometheus.NewSummaryVec(prometheus.SummaryOpts{
		Name:       "message_delivery_time",
		Help:       "Message delivery time",
		Objectives: map[float64]float64{0.5: 0.05, 0.9: 0.1, 0.95: 0.01, 0.99: 0.01},
		MaxAge:     5 * time.Second,
	}, []string{"kind"})
	prometheus.MustRegister(collector.MessageDeliveryTime)

	collector.MessageCounter = prometheus.NewCounterVec(prometheus.CounterOpts{
		Name: "message_counter",
		Help: "Message counters",
	}, []string{"kind", "direction"})
	prometheus.MustRegister(collector.MessageCounter)

	collector.PipeBodyLength = prometheus.NewSummary(prometheus.SummaryOpts{
		Name:       "pipe_body_length",
		Help:       "Pipe body length",
		Objectives: map[float64]float64{0.5: 0.05, 0.9: 0.1, 0.95: 0.01, 0.99: 0.01},
		MaxAge:     1 * time.Minute,
	})
	prometheus.MustRegister(collector.PipeBodyLength)

	collector.PipeDownloadTime = prometheus.NewSummary(prometheus.SummaryOpts{
		Name:       "pipe_download_time",
		Help:       "Pipe download time",
		Objectives: map[float64]float64{0.5: 0.05, 0.9: 0.1, 0.95: 0.01, 0.99: 0.01},
		MaxAge:     1 * time.Minute,
	})
	prometheus.MustRegister(collector.PipeDownloadTime)

	return collector
}

func (collector *StatsCollector) ObserveOutgoingMessage(e *Envelope) {
	timing := float64(time.Since(e.Born) / time.Millisecond)
	collector.MessageDeliveryTime.WithLabelValues("pre-wire").Observe(timing)
	kind := extractEnvelopeMetricKind(e)
	collector.MessageCounter.WithLabelValues(kind, "outgoing").Add(1)
}

func (collector *StatsCollector) ObserveSentMessage(e *Envelope) {
	timing := float64(time.Since(e.Born) / time.Millisecond)
	collector.MessageDeliveryTime.WithLabelValues("wire").Observe(timing)
}

func (collector *StatsCollector) ObserveIncomingMessage(e *Envelope) {
	kind := extractEnvelopeMetricKind(e)
	collector.MessageCounter.WithLabelValues(kind, "incoming").Add(1)
}

func (collector *StatsCollector) ObserveNewClient() {
	collector.CurrentClients.Inc()
	collector.TotalSessionClients.Inc()
}

func (collector *StatsCollector) ObserveDroppedClient() {
	collector.CurrentClients.Sub(1)
}

func (collector *StatsCollector) ResetClients() {
	collector.CurrentClients.Set(0)
	collector.TotalSessionClients.Set(0)
}

func (collector *StatsCollector) ObservePipeLength(length int64) {
	collector.PipeBodyLength.Observe(float64(length))
}

func (collector *StatsCollector) ObservePipeDownloadTime(dl time.Duration) {
	collector.PipeDownloadTime.Observe(float64(dl / time.Millisecond))
}

func (collector *StatsCollector) ListenAndServe(addr string) error {
	server := &http.Server{Addr: addr, Handler: prometheus.Handler()}
	return server.ListenAndServe()
}

func extractEnvelopeMetricKind(e *Envelope) string {
	switch {
	case e.Kind == MsgidPing:
		return "ping"
	case e.Kind < 400:
		return "system"
	case e.Kind >= 400 && e.Kind < 500:
		return "error"
	case e.Kind >= 1000:
		return "game"
	}

	return "unknown"
}
