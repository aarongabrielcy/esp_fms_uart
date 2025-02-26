#ifndef SIM7600_H
#define SIM7600_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string>
#include <sstream>
#include "uartManager.h"
#include "serialConsole.h"
#include "parseData.h"
#include "trackerData.h"

class SIM7600 {
    private:
        SIM7600();
        bool waitingForPrompt;
        bool gpsReportReady;
        bool psiReportReady;
        std::string message;
        bool isModuleReady();
        bool cclkRequested = false;
    public:
        static SIM7600& getInstance();
        void configureSIM7600();
        void sendTCPMessage();
        void processUARTEvent(std::string line);
        bool checkAndReconnectTCP();
        void updateMessage();
};
#endif