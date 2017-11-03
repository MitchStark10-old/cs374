package main

import "fmt"

func main() {
	var verbose string = "Message";
	shorthand := "Another message";
	fmt.Println(verbose);
	fmt.Println(shorthand);
	go helloWorld();
}

func helloWorld() {
	fmt.Println("Hello World");
}


