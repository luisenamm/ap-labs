package main

import (
	"fmt"
	"time"
)

func connection(ping chan int, pong chan int) {
	for {
		pong <- (1 + <-ping)
	}
}

func main() {
	var commsPerSecond int = 0

	ping := make(chan int)
	pong := make(chan int)

	go connection(ping, pong)
	go connection(pong, ping)

	for i := 0; i < 1; i++ {
		ping <- 0
		time.Sleep(time.Duration(1) * time.Second)
		cont := <-ping
		commsPerSecond += cont
	}

	fmt.Println("Communications Per Second : ", commsPerSecond)
}
