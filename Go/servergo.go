package main

import (
	"container/list"
)

type Tupla struct {
	Chave string
	Valor string
}

var (
	listaTuplas = list.New()
)

func main() {

}

func WR(chave string, valor string) {
	listaTuplas.PushBack(Tupla{chave, valor})
}

func RD(chave string) {

}

func IN() {

}

func EX() {

}

func servico1() {

}

func servico2() {

}

func servico3() {

}
