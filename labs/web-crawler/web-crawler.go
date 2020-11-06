// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
//
package main

import (
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"

	"gopl.io/ch5/links"
)

//!+sema
// tokens is a counting semaphore used to
// enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)

type web struct {
	url   string
	depth int
}

//

func crawl(url web, file *os.File) []web {
	if url.depth > 0 {
		_, err := file.WriteString(url.url + "\n")
		if err != nil {
			fmt.Println("Error writing report file")
		}

		tokens <- struct{}{} // acquire a token
		list, err := links.Extract(url.url)
		<-tokens // release the token
		var sites []web
		for _, n := range list {
			sites = append(sites, web{url: n, depth: url.depth - 1})
		}

		if err != nil {
			log.Print(err)
		}
		return sites
	}
	var sites []web
	return sites
}

//!-sema

//!+
func main() {
	worklist := make(chan []web)
	var n int // number of pending sends to worklist
	// Start with the command-line arguments.
	n++

	if len(os.Args) < 3 {
		log.Print("Wrong number of parameters")
		return
	}
	var input []web
	args1 := os.Args[1]
	depthString := args1[len(args1)-1:]
	maxDepth, err := strconv.Atoi(depthString)
	if err != nil {
		log.Print(err)
	}

	args2 := os.Args[2]
	file := strings.Replace(args2, "-results=", "", -1)

	results, err := os.Create(file)
	if err != nil {
		fmt.Println("Error writing report file")
	}
	defer results.Close()

	input = append(input, web{url: os.Args[3], depth: maxDepth})

	go func() { worklist <- input }()

	depth := 0
	// Crawl the web concurrently.
	seen := make(map[web]bool)
	for ; n > 0; n-- {
		list := <-worklist
		if depth <= maxDepth {
			for _, link := range list {
				if !seen[link] {
					seen[link] = true
					n++
					go func(link web) {
						worklist <- crawl(link, results)
					}(link)
				}
			}
			depth++
		}
	}
}

//!-
