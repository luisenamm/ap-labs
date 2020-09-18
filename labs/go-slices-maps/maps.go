package main

import (
	"strings"

	"golang.org/x/tour/wc"
)

func WordCount(s string) map[string]int {
	str := strings.Fields(s)
	mapa := make(map[string]int)
	for _, i := range str {
		mapa[i]++
	}
	return mapa
}

func main() {
	wc.Test(WordCount)
}
