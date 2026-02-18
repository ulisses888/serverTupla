package main

import (
	"fmt"
	"strconv"
	"strings"
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

	mensagens <- Mensagem{
		Operacao: "RD",
		Chave:    "teste",
	}

	close(mensagens)
	wg.Wait()
}

func espacoTuplas(mensagens chan Mensagem, wg *sync.WaitGroup) {
	//	mensagensEx :=make(chan MensagemEX)
	defer wg.Done()

	var lista []Tupla

	for msg := range mensagens {
		switch msg.Operacao {
		case "WR":
			t := Tupla{
				Chave: msg.Chave,
				Valor: msg.Valor,
			}
			lista = append(lista, t)
			fmt.Println(msg.Chave, "Escrito com sucesso")
		case "RD":
			for _, tupla := range lista {
				if tupla.Chave == msg.Chave {
					fmt.Println("Leitura com sucesso: ", tupla.Valor)
				}
			}
		case "IN":
			for i, tupla := range lista {
				if tupla.Chave == msg.Chave {
					fmt.Println("Leitura IN com sucesso:", tupla.Valor)
					lista = append(lista[:i], lista[i+1:]...)
					break
				}
			}
		case "EX":
			servID, err := strconv.Atoi(msg.Servico)
			if err != nil || (servID != 1 && servID != 2 && servID != 3) {
				fmt.Println("Operacao Invalida")
				break
			}
			for _, tupla := range lista {
				if tupla.Chave == msg.Chave {
					switch servID {
					case 1:
						tupla.Valor = strings.ToUpper(tupla.Valor)
						tupla.Chave = msg.ChaveSaida
					case 2:
						//linguagem ferrada que n tem uma funcao de reverse para string
					case 3:
						fmt.Println(len(tupla.Valor))

					}
				}
			}

		default:
			fmt.Println("Operacao invalida")
		}

	}
}
