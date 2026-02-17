#include <iostream>
#include <string>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>
#include <algorithm>

struct Tupla {
    std::string chave;
    std::string valor;
};

std::shared_mutex bloqueador;
std::condition_variable_any cv;
std::list<Tupla> listaTuplas;
unsigned short int listaservicos[] = {1,2,3};

std::string WR(std::string chave, std::string valor);
std::string RD(std::string chave);
std::string IN(std::string chave);
std::string EX(std::string chaveEntrada,std::string chaveSaida,std::string servico);
std::string servico1(std::string valor);
std::string servico2(std::string valor);
std::string servico3(std::string valor);

int main(){
    //std::thread t1(RD,"123");
    //std::thread t2(WR,"123","87654321");
    //std::string teste = "testando maisculo";
    //std::cout << "Antes:  " << teste << std::endl;
    //std::cout << "Depois: " << servico3(teste) << std::endl; 
    //t1.join();
    //t2.join();

    return 0; 
}

std::string WR(std::string chave, std::string valor){
    std::unique_lock lock(bloqueador);
    listaTuplas.emplace_back(Tupla{chave,valor});
    std::cout << "Escreveu a chave:" << chave << std::endl;
    cv.notify_all();
    return "OK";
}

std::string RD(std::string chaveBusca){
    std::shared_lock lock(bloqueador);
    while(true){
        for (auto it = listaTuplas.begin(); it != listaTuplas.end(); ++it) {
            if (it->chave == chaveBusca) {
                std::cout << "Leu com sucesso:" << it->valor << std::endl;
                return ("OK ",it->valor);
            }
        }
    cv.wait(lock);
    }
}

std::string IN(std::string chaveBusca){
    std::unique_lock lock(bloqueador);
    std::string valor =  NULL;

    while(true){
        for (auto it = listaTuplas.begin(); it != listaTuplas.end(); ++it) {
            if (it->chave == chaveBusca) {
                valor = it->valor;
                listaTuplas.erase(it);
                std::cout << "Leu com sucesso:" << it->valor << std::endl;
                return ("OK ",valor);
            }
        }
    cv.wait(lock);
    }
}

std::string EX(std::string chaveEntrada,std::string chaveSaida,std::string servico){
    unsigned short int servicos = std::stoi(servico);
    for(int i = 0; i < 3; i++){
        if(listaservicos[i] == servicos){
            goto temServico;
        }
    }
    return "NO-SERVICE";
    temServico:
    std::unique_lock lock(bloqueador);
    while(true){
        for(auto it = listaTuplas.begin(); it != listaTuplas.end(); ++it){
            if(it->chave == chaveEntrada){
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
        cv.wait(bloqueador);
    }
}

std::string servico1(std::string valor){
    std::transform(valor.begin(),valor.end(),valor.begin(),[](unsigned char c){ return std::toupper(c); });
    return valor;
}

std::string servico2(std::string valor){
    std::reverse(valor.begin(),valor.end());
    return valor;
}

std::string servico3(std::string valor){
    return std::to_string(valor.size());
}