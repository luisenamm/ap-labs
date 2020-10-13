package main

import (
	"io"
	"log"
	"net"
	"os"
)

func copy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}

func main() {
	if len(os.Args) < 2 {
		log.Print("Try:  go run clockWall.go City=localhost:n-port")
		os.Exit(1)
	}
	done := make(chan int)
	for i := 1; i < len(os.Args); i++ {
		str := os.Args[i]
		conn, err := net.Dial("tcp", "localhost:"+str[len(str)-4:])
		if err != nil {
			log.Fatal(err)
		}
		defer conn.Close()
		go copy(os.Stdout, conn)
	}
	x := 1
	x = <-done
	log.Println("Channel closed whit value: ", x)
	close(done)
}
