#include <iostream>
#include <istream>
#include <string>

#include "uci.h"

/*
Implementing the protocol defined at
http://wbec-ridderkerk.nl/html/UCIProtocol.html
*/

namespace uci {

    void UCIEngine::run(){
        std::string cmd;
        while(getline(std::cin, cmd)){
            if(cmd == "uci") { /*unimplemented*/ }
            else if(cmd == "debug") { /*unimplemented*/ }
            else if(cmd == "isready") { /*unimplemented*/ }
            else if(cmd == "setoption") { /*unimplemented*/ }
            else if(cmd == "register") { /*unimplemented*/ }
            else if(cmd == "ucinewgame") { /*unimplemented*/ }
            else if(cmd == "position") { /*unimplemented*/ }
            else if(cmd == "go") { /*unimplemented*/ }
            else if(cmd == "stop") { stop(); }
            else if(cmd == "ponderhit") { /*unimplemented*/ }
            else if(cmd == "quit") { quit(); }

            // Proprietary extensions
            else if(cmd == "display") { quit(); }
            else if(cmd == "perft") { quit(); }

            // default
            else { std::cout << "unknonwn command: " << cmd << std::endl; }
        }
    }

    void UCIEngine::stop(){
    }

    void UCIEngine::quit(){
        exit(0);
    }

}
