// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"time"
)

func handleConn(c net.Conn, tz *time.Location) {
	defer c.Close()
	for {
		str := tz.String() + ": " + time.Now().In(tz).Format("15:04:05\n")
		_, err := io.WriteString(c, str)
		if err != nil {
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	tz := os.Getenv("TZ")
	city, err := time.LoadLocation(tz)
	if err != nil {
		log.Fatal("Can't get the Time Zone")
	}
	if len(os.Args) < 3 {
		log.Fatal("Missing parameters. Use: go run clock2.go -port n-port")
	}

	listener, err := net.Listen("tcp", "localhost: "+os.Args[2])

	if err != nil {
		log.Fatal(err)
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}
		go handleConn(conn, city) // handle connections concurrently
	}
}
