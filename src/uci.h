#ifndef UCI_H_INCLUDED
#define UCI_H_INCLUDED

namespace uci {

    class UCIEngine{
    public:
        void run();

    private:
        void stop();
        void quit();
    };

}

#endif // #ifndef UCI_H_INCLUDED
