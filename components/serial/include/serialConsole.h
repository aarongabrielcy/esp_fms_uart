#ifndef SERIALCONSOLE_H
#define SERIALCONSOLE_H

#include "uartManager.h"

#include <string>

class serialConsole {
    private:
        serialConsole();
    public:
        static serialConsole& getInstance();
        void sendCommand(const std::string &command);

};
#endif