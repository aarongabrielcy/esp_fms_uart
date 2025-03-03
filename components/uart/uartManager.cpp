#include "uartManager.h"

static const char *TAG = "UART_MANAGER";

uartManager::uartManager() : state(IDLE) {}

uartManager& uartManager::getInstance() {
    static uartManager instance;
    return instance;
}

void uartManager::init() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void uartManager::startListening() {
    xTaskCreate(uartTask, "uartTask", 8192, this, 1, NULL);
}

void uartManager::sendData(const std::string &data) {
    //ESP_LOGI(TAG, "SEND data Received: %s", data.c_str());
    uart_write_bytes(UART_NUM, data.c_str(), data.length());
    uart_write_bytes(UART_NUM, "\r\n", 2);
}

void uartManager::uartTask(void *pvParameters) {
    uartManager *instance = static_cast<uartManager*>(pvParameters);
    uint8_t data[BUF_SIZE];
    std::string buffer = "";

    while (true) {
        //////////// cuando mandas comandos desde el ESP al Modilo SIM a veces manda datos "basura", agrega una validacion  a los comandos de configuración
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(100));
        if (len > 0) {
            data[len] = '\0';
            buffer += std::string((char*)data);
            //ESP_LOGI(TAG, "UART Raw Data: %s", buffer.c_str());
            size_t pos;
            while ((pos = buffer.find("\n")) != std::string::npos) {
                std::string line = buffer.substr(0, pos);
                buffer.erase(0, pos + 1);
                ESP_LOGI(TAG, "Procesando línea: [%s]", line.c_str());
                instance->processUartData(line);
            }
        }
    }
}
/*void uartManager::uartTask(void *pvParameters) {
    uartManager *instance = static_cast<uartManager*>(pvParameters);
    uint8_t data[BUF_SIZE] = {0};
    std::string buffer = "";

    while (true) {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(100));
        if (len > 0) {
            if (len >= BUF_SIZE) len = BUF_SIZE - 1;
            data[len] = '\0';

            buffer.append(reinterpret_cast<char*>(data), len);
            
            // Evita que `buffer` crezca indefinidamente
            if (buffer.length() > 512) {
                ESP_LOGW("UART_MANAGER", "Buffer demasiado grande, limpiando...");
                buffer.clear();
            }

            size_t pos;
            while ((pos = buffer.find("\n")) != std::string::npos) {
                std::string line = buffer.substr(0, pos);
                buffer.erase(0, pos + 1);
                instance->processUartData(line);
                vTaskDelay(pdMS_TO_TICKS(5));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}*/
void uartManager::processUartData(std::string line) {
    //ESP_LOGI(TAG, "UART Received: %s", line.c_str());
    lastResponse = line;
    state = RECEIVING;
    state = PROCESSING;
    // Aquí se pueden agregar más procesos para manejar respuestas AT
    SIM7600::getInstance().processUARTEvent(line);
    state = IDLE;
}
//Función para verificar si la última respuesta de UART contiene un texto específico
bool uartManager::lastResponseContains(const std::string& keyword) {
    return lastResponse.find(keyword) != std::string::npos;
}
void uartManager::serialInputTask(void *pvParameters) {
    char inputBuffer[100];

    while (true) {
        int len = fread(inputBuffer, 1, sizeof(inputBuffer) - 1, stdin);
        if (len > 0) {
            inputBuffer[len] = '\0';  // Asegurar que sea una string válida
            std::string inputStr(inputBuffer);
            ESP_LOGI(TAG, "INPUT Received: %s", inputBuffer);
            //Verificar si es un comando de configuración
            /*if (inputStr.find("timereport=") != std::string::npos) {
                int newTime = std::stoi(inputStr.substr(inputStr.find("=") + 1));
                if (newTime >= 1 && newTime <= 255) {
                    SIM7600::getInstance().timeReport = newTime;
                    ESP_LOGI(TAG, "Intervalo de reporte ajustado a %d segundos", SIM7600::getInstance().timeReport);
                    SIM7600::getInstance().updateReportRate(newTime);
                } else {
                    ESP_LOGW(TAG, "Valor fuera de rango (1-255)");
                }
            }
            else if (inputStr.find("anglecourse=") != std::string::npos) {
                int newAngle = std::stoi(inputStr.substr(inputStr.find("=") + 1));
                if (newAngle >= 1 && newAngle <= 90) {
                    SIM7600::getInstance().angleCourse = newAngle;
                    ESP_LOGI(TAG, "Angulo de activacion ajustado a %d°", SIM7600::getInstance().angleCourse);
                } else {
                    ESP_LOGW(TAG, "Valor fuera de rango (1-90)");
                }
            }
            else {*/
                // Si no es comando interno, enviarlo por UART_1
                serialConsole::getInstance().sendCommand(inputStr); //inputBuffer / inputStr
            /*}*/
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void uartManager::startSerialInputTask() {
    xTaskCreate(serialInputTask, "serialInputTask", 4096, NULL, 1, NULL);
}
/*void uartManager::tcpTask(void *pvParameters) {
    ESP_LOGI(TAG, "Tarea TCP activa ~~~~~~~~~~~~~~~~~~~~~~~~");
    while (true) {
       SIM7600::getInstance().sendTCPMessage();
        vTaskDelay(pdMS_TO_TICKS(30000));  // Esperar 30s
    }
}

void uartManager::startTCPTask() {
    xTaskCreate(tcpTask, "tcpTask", 4096, NULL, 1, NULL);
}*/