#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uartManager.h"
#include "serialConsole.h"
#include "pwManager.h"
#include "SIM7600.h"

extern "C" void app_main() {

    pwManager::getInstance().powerModule();
    pwManager::getInstance().powerKey();

    uartManager& uart = uartManager::getInstance();
    //serialConsole& at = serialConsole::getInstance();
    SIM7600& sim7600 = SIM7600::getInstance();
    
    uart.init();
    uart.startListening();
    uart.startSerialInputTask();
    sim7600.configureSIM7600();
    //uart.startTCPTask();
    while (true) {
        //at.sendCommand("AT");
        vTaskDelay(pdMS_TO_TICKS(1000));  // Enviar comando AT cada 5s
    }
}
