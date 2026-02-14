#include <iostream>
#include <string>
#include <list>
#include <mutex>
#include <thread>

struct Tupla {
    std::string chave;
    std::string valor;
};

std::mutex bloqueador;
std::list<Tupla> listaTuplas;

void WR(std::string chave, std::string valor);
std::string RD(std::string chave);
std::string IN(std::string chave);

int main(){
    
    return 0; 
}

void WR(std::string chave, std::string valor){

    listaTuplas.emplace_front(Tupla{chave,valor});

    return;
}

std::string RD(std::string chaveBusca){
    
    for (auto it = listaTuplas.rbegin(); it != listaTuplas.rend(); ++it) {
        if (it->chave == chaveBusca) {
            return it->valor;
        }
    }
    return "erro";
} 