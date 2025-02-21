#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uartManager.h"
#include "serialConsole.h"
#include "pwManager.h"



extern "C" void app_main() {

    pwManager::getInstance().powerModule();
    pwManager::getInstance().powerKey();

    uartManager& uart = uartManager::getInstance();
    serialConsole& at = serialConsole::getInstance();
    
    uart.init();
    uart.startListening();
    uart.startSerialInputTask();

    while (true) {
        //at.sendCommand("AT");
        vTaskDelay(pdMS_TO_TICKS(1000));  // Enviar comando AT cada 5s
    }
}
