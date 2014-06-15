package main

import (
	"log"
)


func bufferMessages(src, dst chan Message, delegator chan chan Message) {
	var buffer []Message
	lastm := <-src
	for {
		select {
		case m := <-src:
			// recieved new message from registry, buffer it
			buffer = append(buffer, m)
		case dst <- lastm:
			if len(buffer) == 0 {
				// resend is complete. Send original channel to client and exit
				log.Println("queue: resending is complete, delegating control")
				delegator <- src
				close(dst)
				return
			} else {
				// take next buffered message
				lastm = buffer[0]
				buffer = buffer[1:]
			}
		}
	}
}
