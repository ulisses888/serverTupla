package main

import (
	"fmt"
)

type Tupla struct {
	Chave string
	Valor string
}

type Mensagem struct{
	Operacao string
	Chave string
	ChaveSaida string //aq pro ex
	Valor string
	Servico string //aq pro ex tbm
}
//type MensagemEX struct{
//	Operacao string
//	Chave string
//	Valor string
//	Servico string
//}

func main(){
fmt.println("Hello world!")
mensagens <- Mensagem{
	Operacao:   "WR",
	Chave:      "teste",
	Valor:      "12345",
	ChaveSaida: "",
	Servico:    "",
}
}


func espacoTuplas(){
	mensagens :=make(chan Mensagem,300)
//	mensagensEx :=make(chan MensagemEX)

	for msg := range mensagens {
		switch msg.Operacao{
		case "WR":
			fmt.Println("WR");
		case "RD":
			fmt.Println("RD")		
		}

}