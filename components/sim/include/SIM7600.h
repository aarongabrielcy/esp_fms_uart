#ifndef SIM7600_H
#define SIM7600_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string>
#include "uartManager.h"
#include "serialConsole.h"

class SIM7600 {
    private:
        SIM7600();
        bool waitingForPrompt;
        std::string message;
        bool isModuleReady();
        bool gpsReportReady = false;
        bool psiReportReady = false;
    public:
        static SIM7600& getInstance();
        void configureSIM7600();
        void sendTCPMessage();
        void processUARTEvent(std::string line);
        bool checkAndReconnectTCP();
};
#endif