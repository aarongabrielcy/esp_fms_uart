#ifndef UARTMANAGER_H
#define UARTMANAGER_H

#include "driver/uart.h"
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "serialConsole.h"

#define UART_NUM UART_NUM_1
#define TXD_PIN 4
#define RXD_PIN 5
#define BUF_SIZE 1024

enum uartState {
    IDLE,
    RECEIVING,
    PROCESSING
};

class uartManager {
    private:
        uartManager();
        uartState state;
        void processUartData(std::string line);
        static void uartTask(void *pvParameters);
        static void serialInputTask(void *pvParameters);
    public:
        static uartManager& getInstance();
        void init();
        void startListening();
        void sendData(const std::string &data);
        void startSerialInputTask();
};


#endif