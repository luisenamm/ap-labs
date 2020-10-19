// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

var (
	entering      = make(chan client)
	leaving       = make(chan client)
	messages      = make(chan string)     // all incoming client messages
	clients       = make(map[client]bool) // all connected clients
	users         = make(map[string]bool)
	date          = make(map[string]string)
	clientChannel = make(map[string]chan string)
	connections   = make(map[string]net.Conn)
	admin         = ""
)

func broadcaster() {
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)
	server := "irc-server > "

	user := make([]byte, 100)
	_, err := conn.Read(user)
	name := ""
	if err != nil {
		conn.Close()
		log.Print(err)
	}

	for i := 0; i < len(user); i++ {
		if user[i] == 0 {
			break
		}
		name = name + string(user[i])
	}

	if val, state := users[name]; val && state {
		ch <- server + "User is on the server already"
		conn.Close()
	} else {
		ch <- server + "Welcome to the Simple IRC Server"
		users[name] = true
		clientChannel[name] = ch
		connections[name] = conn
		date[name] = time.Now().Format("2006-01-02 15:04:05")

		ch <- server + "Your user [" + name + "] is succesfully logged!"
		if len(users) == 1 {
			ch <- server + "Congrats, you were the first user."
		}
		if admin == "" {
			admin = name
			ch <- server + "You're the new IRC Server ADMIN"
			fmt.Println(server + "[" + name + "] was promoted as the channel ADMIN")
		}

		messages <- server + "New connected user [" + name + "]"
		fmt.Println(server + "New connected user [" + name + "]")
		entering <- ch

		input := bufio.NewScanner(conn)
		for input.Scan() {
			commands := strings.Split(input.Text(), " ")
			if commands[0] == "/users" {
				for i := range users {
					ch <- server + i + " connected since " + date[i]
				}
			} else if commands[0] == "/msg" {
				if len(commands) > 1 {
					if len(commands) > 2 {
						if to, ok := users[commands[1]]; to && ok {
							message := ""
							for i := 2; i < len(commands); i++ {
								message = message + commands[i] + " "
							}
							clientChannel[commands[1]] <- "[" + name + "] sent you a message: " + message
						} else {
							ch <- server + "User disconnected or non-existent"
						}
					} else {
						ch <- server + "Write a message"
					}
				} else {
					ch <- server + "Wrong usage of msg command"
				}
			} else if commands[0] == "/time" {
				local, err := time.LoadLocation("Local")
				if err != nil {
					log.Fatal("Error getting TZ")
				}
				if local.String() == "Local" {
					ch <- server + "Local Time: America/Mexico_City" + " " + time.Now().Format("15:04") //strconv.Itoa(time.Now().Hour())
				} else {
					ch <- server + "Local Time: " + local.String() + " " + time.Now().Format("15:04") //strconv.Itoa(time.Now().Hour())
				}
			} else if commands[0] == "/user" {
				if len(commands) > 1 {
					if username, ok := users[commands[1]]; username && ok {
						ch <- server + "username: " + commands[1] + ", IP: " + connections[commands[1]].RemoteAddr().String() + " Connected since: " + date[commands[1]]
					} else {
						ch <- server + "User disconnected or non-existent"
					}
				} else {
					ch <- server + "Write an user"
				}
			} else if commands[0] == "/kick" {
				if len(commands) > 1 {
					if name == admin {
						if username, ok := users[commands[1]]; username && ok {
							clientChannel[commands[1]] <- server + "You were kicked from this channel"
							clientChannel[commands[1]] <- server + "Bad language is not allowed on this channel"
							connections[commands[1]].Close()
							messages <- server + "[" + commands[1] + "] was kicked from channel for bad language policy violation"
							fmt.Println(server + "[" + commands[1] + "] was kicked from channel for bad language policy violation")
						} else {
							ch <- server + "User disconnected or non-existent"
						}
					} else {
						ch <- server + "/kick command is only allowed for admins"
					}
				} else {
					ch <- server + "Write an user to kick"
				}
			} else if string(commands[0][0]) == "/" {
				ch <- server + "Command does not exist"
			} else {
				messages <- name + " > " + input.Text()
			}
		}
	}

	// NOTE: ignoring potential errors from input.Err()

	leaving <- ch
	messages <- name + " has left"
	fmt.Println(server + "[" + name + "] has left")
	conn.Close()
	users[name] = false
	if admin == name {
		for i := range users {
			if users[i] == true {
				admin = i
				clientChannel[i] <- server + "You're the new IRC Server Admin"
				break
			}
		}
		if admin == name {
			admin = ""
		}
	}
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {

	if len(os.Args) != 5 {
		log.Fatal("Wrong number of parameters")
	}

	listener, err := net.Listen("tcp", os.Args[2]+":"+os.Args[4])
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("irc-server > Simple IRC Server started at " + os.Args[2] + ":" + os.Args[4])
	fmt.Println("irc-server > Ready for receaving new clients")
	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
