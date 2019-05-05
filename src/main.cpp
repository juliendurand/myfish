#include "iostream"

#include "uci.h"

int main(){
    std::cout << "Myfish by Julien Durand" << std::endl;

    uci::UCIEngine engine;
    engine.run();
}
