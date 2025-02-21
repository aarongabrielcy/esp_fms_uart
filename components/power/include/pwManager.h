#ifndef PWMANAGER_H
#define PWMANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"

#define POWER_KEY_PIN 41
#define POWER_SIM_PIN 38
#define GNSS_LED_PIN  19
#define POWER_LED_PIN 20
#define SIM_DTR_PIN   42
#define INPUT_IGN     10

class pwManager {
    private:
        pwManager() = default;
        unsigned long previousMillis = 0;
        unsigned long ledInterval = 200; // Intervalo para el parpadeo del LED
        int ledState = 0;                // Estado actual del LED
        int pinIgn;                      // Pin de ignición
    public:
        static pwManager& getInstance();
        void powerKey();
        void powerModule();
        void powerLedGnss();
        void blinkLedGnss(int fixState);
        void initInIgn(int pin);
        bool getStateIgn();
        void OffModule();
        void restartMicro();   

};
#endif