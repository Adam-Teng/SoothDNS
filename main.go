package main

import (
	"fmt"

	"soothDNS/args"
)

func main() {
	// Instantiate an ArgParser instance.
	parser := args.NewParser()
	parser.Helptext = "Usage: example..."
	parser.Version = "1.2.3"

	// Register a flag and a string-valued option.
	parser.NewFlag("foo f")
	parser.NewStringOption("bar b", "fallback")

	// Parse the command line arguments.
	parser.Parse()
	fmt.Println(parser)
}
