#include <iostream>
#include <string>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>
#include <algorithm>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
const int PORTA = 54321;

struct Tupla
{
    std::string chave;
    std::string valor;
};

std::shared_mutex bloqueador;
std::condition_variable_any cv;
std::list<Tupla> listaTuplas;
unsigned short int listaservicos[] = {1, 2, 3};

std::string WR(std::string chave, std::string valor);
std::string RD(std::string chave);
std::string IN(std::string chave);
std::string EX(std::string chaveEntrada, std::string chaveSaida, std::string servico);
std::string servico1(std::string valor);
std::string servico2(std::string valor);
std::string servico3(std::string valor);
void handle_client(int client_fd);
std::string processar_comando(const std::string& linha);

int main(int argc, char *argv[])
{

    // if (argc != 2)
    //{
    //     std::cerr << "Uso: " << argv[0] << " <porta>\n";
    //     std::cerr << "Exemplo: " << argv[0] << " 12345\n";
    //     return 1;
    // }
    // int port = std::stoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        return 1;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORTA);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        return 1;
    }

    std::cout << "Servidor escutando na porta " << PORTA << "...\n";

    while (true) {
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        perror("accept");
        continue;
    }

    std::cout << "Cliente conectado!\n";

    std::thread t(handle_client, client_fd);
    t.detach(); // deixa rodando independente
}

    return 0;
}

std::string WR(std::string chave, std::string valor)
{
    std::unique_lock lock(bloqueador);
    listaTuplas.emplace_back(Tupla{chave, valor});
    //std::cout << "Escreveu a chave:" << chave << std::endl;
    cv.notify_all();
    return "OK";
}

std::string RD(std::string chaveBusca)
{
    std::shared_lock lock(bloqueador);
    while (true)
    {
        for (auto it = listaTuplas.begin(); it != listaTuplas.end(); ++it)
        {
            if (it->chave == chaveBusca)
            {
                //std::cout << "Leu com sucesso:" << it->valor << std::endl;
                return ("OK " + it->valor);
            }
        }
        cv.wait(lock);
    }
}

std::string IN(std::string chaveBusca)
{
    std::unique_lock lock(bloqueador);

    while (true)
    {
        for (auto it = listaTuplas.begin(); it != listaTuplas.end(); ++it)
        {
            if (it->chave == chaveBusca)
            {
                std::string valor = it->valor;
                //std::cout << "Leu com sucesso:" << it->valor << std::endl;
                listaTuplas.erase(it);
                return ("OK " + valor);
            }
        }
        cv.wait(lock);
    }
}

std::string EX(std::string chaveEntrada, std::string chaveSaida, std::string servico)
{
    unsigned short int servicos = std::stoi(servico);
    for (int i = 0; i < 3; i++)
    {
        if (listaservicos[i] == servicos)
        {
            goto temServico;
        }
    }
    return "NO-SERVICE";
temServico:
    std::unique_lock lock(bloqueador);
    while (true)
    {
        for (auto it = listaTuplas.begin(); it != listaTuplas.end(); ++it)
        {
            if (it->chave == chaveEntrada)
            {
                it->chave = chaveSaida;
                switch (servicos)
                {
                case 1:
                    it->valor = servico1(it->valor);
                    break;
                case 2:
                    it->valor = servico2(it->valor);
                    break;
                case 3:
                    it->valor = servico3(it->valor);
                    break;
                }
                return "OK";
            }
        }
        cv.wait(lock);
    }
}

std::string servico1(std::string valor)
{
    std::transform(valor.begin(), valor.end(), valor.begin(), [](unsigned char c)
                   { return std::toupper(c); });
    return valor;
}

std::string servico2(std::string valor)
{
    std::reverse(valor.begin(), valor.end());
    return valor;
}

std::string servico3(std::string valor)
{
    return std::to_string(valor.size());
}

void handle_client(int client_fd) {
    char buffer[1024];
    std::string acumulado;

    while (true) {
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0);

        if (bytes <= 0) {
            break;
        }

        acumulado.append(buffer, bytes);

        size_t pos;
        while ((pos = acumulado.find('\n')) != std::string::npos) {

            std::string linha = acumulado.substr(0, pos);
            acumulado.erase(0, pos + 1);

            std::string resposta = processar_comando(linha);
            resposta += "\n";

            send(client_fd, resposta.c_str(), resposta.size(), 0);
        }
    }

    close(client_fd);
}

std::string processar_comando(const std::string& linha) {

    std::istringstream iss(linha);
    std::string cmd;
    iss >> cmd;

    if (cmd == "WR") {
        std::string chave, valor;
        iss >> chave >> valor;
        if (chave.empty() || valor.empty())
            return "ERROR";
        return WR(chave, valor);
    }

    if (cmd == "RD") {
        std::string chave;
        iss >> chave;
        if (chave.empty())
            return "ERROR";
        return RD(chave);
    }

    if (cmd == "IN") {
        std::string chave;
        iss >> chave;
        if (chave.empty())
            return "ERROR";
        return IN(chave);
    }

    if (cmd == "EX") {
        std::string k_in, k_out, svc;
        iss >> k_in >> k_out >> svc;
        if (k_in.empty() || k_out.empty() || svc.empty())
            return "ERROR";
        return EX(k_in, k_out, svc);
    }

    return "ERROR";
}