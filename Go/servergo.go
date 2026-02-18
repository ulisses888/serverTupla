package main

import (
	"fmt"
	"sync"
)

type Tupla struct {
	Chave string
	Valor string
}

type Mensagem struct {
	Operacao   string
	Chave      string
	ChaveSaida string //aq pro ex
	Valor      string
	Servico    string //aq pro ex tbm
}

//type MensagemEX struct{
//	Operacao string
//	Chave string
//	Valor string
//	Servico string
//}

func main() {
	var wg sync.WaitGroup
	wg.Add(1)
	fmt.Println("Hello world!")
	mensagens := make(chan Mensagem, 300)

	go espacoTuplas(mensagens, &wg)

	mensagens <- Mensagem{
		Operacao:   "WR",
		Chave:      "teste",
		Valor:      "12345",
		ChaveSaida: "",
		Servico:    "",
	}
	close(mensagens)
	wg.Wait()
}

func espacoTuplas(mensagens chan Mensagem, wg *sync.WaitGroup) {
	//	mensagensEx :=make(chan MensagemEX)
	defer wg.Done()
	for msg := range mensagens {
		switch msg.Operacao {
		case "WR":
			fmt.Println("WR")
		case "RD":
			fmt.Println("RD")
		}

	}

}
