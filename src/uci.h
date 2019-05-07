#ifndef UCI_H_INCLUDED
#define UCI_H_INCLUDED

#include <string>
#include <vector>

#include "position.h"

namespace uci {

    class UCIEngine{

    bool debug = false;
    chess::Position position;

    public:
        void run();

    private:
        void uci(const std::string &params);
        void uci_debug(const std::string &params);
        void uci_isready(const std::string &params);
        void uci_setoption(const std::string &params);
        void uci_register(const std::string &params);
        void uci_newgame(const std::string &params);
        void uci_position(const std::string &params);
        void uci_go(const std::string &params);
        void uci_stop(const std::string &params);
        void uci_ponderhit(const std::string &params);
        void uci_quit(const std::string &params);

        // Proprietary extensions
        void display(const std::string &params);
        void perft(const std::string &params);
    };

}

#endif // #ifndef UCI_H_INCLUDED
