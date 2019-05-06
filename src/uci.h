#ifndef UCI_H_INCLUDED
#define UCI_H_INCLUDED

#include <string>
#include <vector>

namespace uci {

    class UCIEngine{

    bool debug = false;

    public:
        void run();

    private:
        void uci(const std::vector<std::string> &params);
        void uci_debug(const std::vector<std::string> &params);
        void uci_isready(const std::vector<std::string> &params);
        void uci_setoption(const std::vector<std::string> &params);
        void uci_register(const std::vector<std::string> &params);
        void uci_newgame(const std::vector<std::string> &params);
        void uci_position(const std::vector<std::string> &params);
        void uci_go(const std::vector<std::string> &params);
        void uci_stop(const std::vector<std::string> &params);
        void uci_ponderhit(const std::vector<std::string> &params);
        void uci_quit(const std::vector<std::string> &params);

        // Proprietary extensions
        void display(const std::vector<std::string> &params);
        void perft(const std::vector<std::string> &params);
    };

}

#endif // #ifndef UCI_H_INCLUDED
