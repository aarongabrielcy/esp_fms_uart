#include "serialConsole.h"

serialConsole::serialConsole() {}

serialConsole& serialConsole::getInstance() {
    static serialConsole instance;
    return instance;
}

void serialConsole::sendCommand(const std::string &command) {
    uartManager::getInstance().sendData(command);
}