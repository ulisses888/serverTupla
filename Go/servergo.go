package main

import (
	"bufio"
	"fmt"
	"net"
	"strconv"
	"strings"
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
	Retorno    chan string
}

//type MensagemEX struct{
// 	Operacao string
// 	Chave string
// 	Valor string
// 	Servico string
//}

type Pendente struct {
	Operacao   string
	Chave      string
	ChaveSaida string
	Servico    string
	Retorno    chan string
}

func main() {
	fmt.Println("Servidor Go rodando na porta 54321...")
	mensagens := make(chan Mensagem, 300)

	go espacoTuplas(mensagens)

	listener, err := net.Listen("tcp", ":54321")
	if err != nil {
		fmt.Println("Erro ao iniciar servidor:", err)
		return
	}
	defer listener.Close()

	for {
		conn, err := listener.Accept()
		if err != nil {
			continue
		}
		go handleClient(conn, mensagens)
	}
}

func espacoTuplas(mensagens chan Mensagem) {
	var lista []Tupla
	var filaEspera []Pendente

	for msg := range mensagens {

		switch msg.Operacao {
		case "WR":
			lista = append(lista, Tupla{Chave: msg.Chave, Valor: msg.Valor})
			if msg.Retorno != nil {
				msg.Retorno <- "OK"
			}
		case "RD", "IN", "EX":
			if msg.Operacao == "EX" && msg.Servico != "1" && msg.Servico != "2" && msg.Servico != "3" {
				msg.Retorno <- "NO-SERVICE"
				continue
			}

			filaEspera = append(filaEspera, Pendente{
				Operacao:   msg.Operacao,
				Chave:      msg.Chave,
				ChaveSaida: msg.ChaveSaida,
				Servico:    msg.Servico,
				Retorno:    msg.Retorno,
			})
		}

		mudou := true
		for mudou {
			mudou = false
			var pendentesRestantes []Pendente

			for i, p := range filaEspera {
				idx := -1
				for j, t := range lista {
					if t.Chave == p.Chave {
						idx = j
						break
					}
				}

				if idx != -1 {
					tupla := lista[idx]

					if p.Operacao == "RD" {
						p.Retorno <- "OK " + tupla.Valor
					} else if p.Operacao == "IN" {
						p.Retorno <- "OK " + tupla.Valor
						lista = append(lista[:idx], lista[idx+1:]...)
						mudou = true
					} else if p.Operacao == "EX" {
						novoValor := executarServico(p.Servico, tupla.Valor)
						lista = append(lista[:idx], lista[idx+1:]...)
						lista = append(lista, Tupla{Chave: p.ChaveSaida, Valor: novoValor})
						p.Retorno <- "OK"
						mudou = true
					}

					if mudou {
						pendentesRestantes = append(pendentesRestantes, filaEspera[i+1:]...)
						break
					}
				} else {

					pendentesRestantes = append(pendentesRestantes, p)
				}
			}
			filaEspera = pendentesRestantes
		}
	}
}

func executarServico(servico, valor string) string {
	switch servico {
	case "1":
		return strings.ToUpper(valor)
	case "2":
		r := []rune(valor)
		for i, j := 0, len(r)-1; i < j; i, j = i+1, j-1 {
			r[i], r[j] = r[j], r[i]
		}
		return string(r)
	case "3":
		return strconv.Itoa(len(valor))
	}
	return ""
}

func handleClient(conn net.Conn, canalPrincipal chan Mensagem) {
	defer conn.Close()
	scanner := bufio.NewScanner(conn)

	for scanner.Scan() {
		linha := scanner.Text()
		parts := strings.Fields(linha)
		if len(parts) == 0 {
			continue
		}

		cmd := parts[0]
		resposta := "ERROR"

		switch cmd {
		case "WR":
			if len(parts) >= 3 {
				meuRetorno := make(chan string)
				canalPrincipal <- Mensagem{
					Operacao: cmd,
					Chave:    parts[1],
					Valor:    parts[2],
					Retorno:  meuRetorno,
				}
				resposta = <-meuRetorno
			}
		case "RD", "IN":
			if len(parts) >= 2 {
				meuRetorno := make(chan string)
				canalPrincipal <- Mensagem{
					Operacao: cmd,
					Chave:    parts[1],
					Retorno:  meuRetorno,
				}
				resposta = <-meuRetorno
			}

		case "EX":
			if len(parts) >= 4 {
				meuRetorno := make(chan string)
				canalPrincipal <- Mensagem{
					Operacao:   cmd,
					Chave:      parts[1],
					ChaveSaida: parts[2],
					Servico:    parts[3],
					Retorno:    meuRetorno,
				}
				resposta = <-meuRetorno
			}
		}

		conn.Write([]byte(resposta + "\n"))
	}
}
