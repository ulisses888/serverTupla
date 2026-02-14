#include <iostream>
#include <string>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>

struct Tupla {
    std::string chave;
    std::string valor;
};

std::shared_mutex bloqueador;
std::condition_variable cv;
std::list<Tupla> listaTuplas;

void WR(std::string chave, std::string valor);
std::string RD(std::string chave);
std::string IN(std::string chave);

int main(){
    
    return 0; 
}

void WR(std::string chave, std::string valor){
    std::unique_lock lock(bloqueador);
    listaTuplas.emplace_back(Tupla{chave,valor});
    std::unique_lock unlock(bloqueador);
    //lembrar d notificar
    return;
}

std::string RD(std::string chaveBusca){
    std::shared_lock lock(bloqueador);
    for (auto it = listaTuplas.begin(); it != listaTuplas.end(); ++it) {
        if (it->chave == chaveBusca) {
            return it->valor;
        }
    }
    lock.unlock();
    //cv.wait();
    return "erro";
}

std::string IN(std::string chaveBusca){
    std::unique_lock lock(bloqueador);
    std::string valor =  NULL;
    for (auto it = listaTuplas.begin(); it != listaTuplas.end(); ++it) {
        if (it->chave == chaveBusca) {
            valor = it->valor;
            listaTuplas.erase(it);
            return valor;
        }
    }
    lock.unlock();
    //aq vai ficar esperando tbm
    return "blabla";
}