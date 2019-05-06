#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

#include "uci.h"

/*
Implementing the protocol defined at
http://wbec-ridderkerk.nl/html/UCIProtocol.html
*/

namespace uci {

    std::string trim(const std::string &s)
    {
       auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
       auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
       return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
    }

    std::string remove_duplicate_whitespaces(const std::string &s){
        std::string str = s;
        std::string::iterator new_end = std::unique(str.begin(), str.end(),
            [](char lhs, char rhs){ return isspace(lhs) && isspace(rhs); }
        );
        str.erase(new_end, str.end());
        return str;
    }

    void UCIEngine::run(){
        std::string line;
        while(getline(std::cin, line)){
            line = remove_duplicate_whitespaces(line);
            line = trim(line);
            std::stringstream ss = std::stringstream(line);
            std::string token;
            std::string cmd;
            std::vector<std::string> params;

            getline(ss, cmd, ' ');
            while(getline(ss, token, ' ')){
                params.push_back(token);
            }

            if(cmd == "uci") { uci(params); }
            else if(cmd == "debug") { uci_debug(params); }
            else if(cmd == "isready") { uci_isready(params); }
            else if(cmd == "setoption") { uci_setoption(params); }
            else if(cmd == "register") { uci_register(params); }
            else if(cmd == "ucinewgame") { uci_newgame(params); }
            else if(cmd == "position") { uci_position(params); }
            else if(cmd == "go") { uci_go(params); }
            else if(cmd == "stop") { uci_stop(params); }
            else if(cmd == "ponderhit") { uci_ponderhit(params); }
            else if(cmd == "quit") { uci_quit(params); }

            // Proprietary extensions
            else if(cmd == "display") { /*unimplemented*/  }
            else if(cmd == "perft") { /*unimplemented*/  }

            // default
            else { std::cout << "unknonwn command: " << cmd << std::endl; }
        }
    }

    void UCIEngine::uci(const std::vector<std::string> &params){
        std::cout << "id name Myfish" << std::endl;
        std::cout << "id author Julien Durand" << std::endl;
        std::cout << "uciok" << std::endl;
    }

    void UCIEngine::uci_debug(const std::vector<std::string> &params){
        for(std::string p : params){
            if(p == "on") { debug = true; break; }
            else if(p == "off") { debug = false; break; }
        }
    }

    void UCIEngine::uci_isready(const std::vector<std::string> &params){
        std::cout << "readyok" << std::endl;
    }

    void UCIEngine::uci_setoption(const std::vector<std::string> &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_register(const std::vector<std::string> &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_newgame(const std::vector<std::string> &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_position(const std::vector<std::string >&params){
        for(std::string p : params){
            if(p == "startpos") { /*unimplemented*/ break; }
            else if(p == "fen") { /*unimplemented*/ break; }
        }
        for(std::string p : params){
            if(p == "moves") { /*unimplemented*/ break; }
        }
    }

    void UCIEngine::uci_go(const std::vector<std::string> &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_stop(const std::vector<std::string> &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_ponderhit(const std::vector<std::string> &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_quit(const std::vector<std::string> &params){
        exit(0);
    }

    void UCIEngine::display(const std::vector<std::string> &params){
        /*unimplemented*/
    }

    void UCIEngine::perft(const std::vector<std::string> &params){
        /*unimplemented*/
    }

}
