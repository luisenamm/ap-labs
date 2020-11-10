//I worked with 1,000,000 pipes
package main

import (
	"fmt"
	"runtime"
	"time"
)

var maxStages int
var transitTime float64
var first chan int

//https://golangcode.com/print-the-current-memory-usage/
func printMemUsage() {
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	if m.TotalAlloc/1024/1024 > 1024 {
		fmt.Sprintf("Out of memory")
	}
}

func pipes(maxPipes int) {
	first = make(chan int)
	last := first
	i := 0
	for i < maxPipes {
		maxStages++
		printMemUsage()
		aux := last
		go func() {
			for data := range aux {
				last <- data
			}
		}()
		i++
	}
}

func testPipes() {
	start := time.Now()
	pipes(1000000)
	finish := time.Now()
	transitTime = finish.Sub(start).Seconds()
}

func main() {
	testPipes()
	fmt.Println("Maximum number of pipeline stages: ", maxStages)
	fmt.Println("Total time to traverse the entire pipeline: ", transitTime, " seconds")

}
