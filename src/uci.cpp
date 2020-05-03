#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <sstream>

#include "uci.h"
#include "search.h"

/*
Implementing the protocol defined at
http://wbec-ridderkerk.nl/html/UCIProtocol.html
*/

namespace uci {

    std::string trim(const std::string &s)
    {
       auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c) || c == '\r' || c == '\n';});
       auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c)|| c == '\r' || c == '\n';}).base();
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

    std::string join(const std::vector<std::string> &v, char sep){
        std::stringstream ss;
        const int v_size = v.size();
        for(size_t i = 0; i < v_size; ++i)  // v is your vector of string
        {
          if(i != 0)
            ss << " ";
          ss << v[i];
        }
        return ss.str();
    }



    void UCIEngine::run(){
        position = new chess::Position();
        std::string line;
        while(getline(std::cin, line)){
            line = remove_duplicate_whitespaces(line);
            line = trim(line);
            std::stringstream ss = std::stringstream(line);
            std::string token;
            std::string cmd;
            std::string params;

            getline(ss, cmd, ' ');
            params = ss.str();

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
            else if(cmd == "display") { display(params); }
            else if(cmd == "eval") { eval(params); }
            else if(cmd == "fen") { fen(params); }
            else if(cmd == "movegen") { movegen(params); }
            else if(cmd == "perft") { perft(params); }

            // default
            else { std::cout << "unknonwn command: " << cmd << std::endl; }
        }
    }

    void UCIEngine::uci(const std::string &params){
        std::cout << "id name Myfish" << std::endl;
        std::cout << "id author Julien Durand" << std::endl;
        std::cout << "uciok" << std::endl;
    }

    void UCIEngine::uci_debug(const std::string &params){
        std::stringstream ss = std::stringstream(params);
        std::string param;
        for(;;){
            getline(ss, param, ' ');
            if(param == "on") { debug = true; break; }
            else if(param == "off") { debug = false; break; }
        }
    }

    void UCIEngine::uci_isready(const std::string &params){
        std::cout << "readyok" << std::endl;
    }

    void UCIEngine::uci_setoption(const std::string &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_register(const std::string &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_newgame(const std::string &params){
        position->reset();
    }

    void UCIEngine::uci_position(const std::string &params){
        std::stringstream ss = std::stringstream(params);
        std::string param;
        for(;;){
            getline(ss, param, ' ');
            if(param == "startpos") {
                position->set_start_position();
                break;
            } else if(param == "fen") {
                getline(ss, param);
                position->import_fen(param);
                break;
            }
        }
        ss = std::stringstream(params);
        while(getline(ss, param, ' ')){
            if(param == "moves") {
                while(getline(ss, param, ' ')){
                    chess::Move m = position->get_move_from_long_algebraic(param);
                    position->make_move(&m);
                }
            }
        }
    }

    void UCIEngine::uci_go(const std::string &params){
        std::cout << "bestmove " <<  chess::search(position, 4) + "\n";;
    }

    void UCIEngine::uci_stop(const std::string &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_ponderhit(const std::string &params){
        /*unimplemented*/
    }

    void UCIEngine::uci_quit(const std::string &params){
        exit(0);
    }

    void UCIEngine::display(const std::string &params){
        const std::string rank_separator = "+---+---+---+---+---+---+---+---+";
        const std::string file_separator = "|";
        std::string sb = position->export_fen() + "\n";
        for(int i = 0; i < 64; i++){
            if(i % 8 == 0){
                if(i > 0) {
                    sb += "\n";
                }
                sb += rank_separator + "\n";
                sb += file_separator;
            }
            int square = i % 8 + (7 - i / 8) * 8;
            sb += " " + position->get_square(square) + " ";
            sb += file_separator;
        }
        sb += "\n" + rank_separator + "\n";
        sb += "Turn: " + std::string(1, position->get_turn()) + "\n";
        sb += "Castling rights: " + position->get_all_castling() + "\n";
        sb += "En Passant: " + position->get_en_passant() + "\n";
        sb += "Nb reversible plies: " +
            std::to_string(position->get_reversible_plies()) + "\n";
        sb += "Moves: " + std::to_string(position->get_move()) + "\n";
        sb += "Plies: " + std::to_string(position->get_plies()) + "\n";
        std::cout << sb;
    }

    void UCIEngine::eval(const std::string &params){
        std::cout << chess::eval(position);
    }

    void UCIEngine::fen(const std::string &params){
        std::cout << position->export_fen() << std::endl;
    }

    void UCIEngine::movegen(const std::string &params){
        /*for(int i=0; i < 1000000; i++){
            chess::MoveGenerator* generator = new chess::MoveGenerator(position);
            generator->generate();
        }*/

        chess::MoveGenerator* generator = new chess::MoveGenerator(position);
        generator->generate();
        for(chess::Move m : generator->moveList){
            std::cout << m.to_long_algebraic() << std::endl;
        }
        delete generator;
    }

    void UCIEngine::perft(const std::string &params){
        std::stringstream ss = std::stringstream(params);
        std::string param;
        getline(ss, param, ' ');
        getline(ss, param, ' ');
        int depth = std::stoi(param);

        auto start = std::chrono::steady_clock::now();
        int nodes = chess::perft(depth, position, true);
        auto end = std::chrono::steady_clock::now();
        float duration = float(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / 1000000;
        std::cout << std::endl << "Searched " << nodes <<  " nodes in " << duration << "s (" << float(nodes) / 1000 / duration << " kNodes/s)." << std::endl << std::endl;
    }

}
