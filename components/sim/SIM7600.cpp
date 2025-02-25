#include "SIM7600.h"

static const char *TAG = "SIM7600";

SIM7600::SIM7600() : waitingForPrompt(false),gpsReportReady(false), psiReportReady(false) {}

SIM7600& SIM7600::getInstance() {
    static SIM7600 instance;
    return instance;
}

void SIM7600::updateMessage() {
    std::ostringstream msgStream;
    msgStream << tkr.header << ";" << tkr.imei << ";" << tkr.rep_map << ";"<< tkr.model << ";" << tkr.sw_ver << ";" << tkr.msg_type << ";"<< tkr.date << ";" << tkr.time << ";" << tkr.cell_id 
              << ";" << tkr.mcc << ";"<< tkr.mnc << ";" << tkr.lac_tac << ";" << tkr.rxlvl_rsrp << ";"<< tkr.lat << ";" << tkr.lon << ";" << tkr.speed << ";"<< tkr.course << ";" << tkr.gps_svs
              << ";" << tkr.fix << ";"<< tkr.in_state << ";" << tkr.out_state << ";" << tkr.mode << ";"<< tkr.stt_rpt_type << ";" << tkr.msg_num << ";" << tkr.bck_volt << ";"<< tkr.power_Volt;
    message = msgStream.str();
    ESP_LOGI(TAG, "📡 Mensaje actualizado: %s", message.c_str());
}

bool SIM7600::isModuleReady() {
    ESP_LOGI(TAG, "Esperando RDY del SIM7600...");
    
    int wait_rdy_retries = 20;  // Máximo 20 intentos (~20 segundos)
    while (wait_rdy_retries--) {
        if (uartManager::getInstance().lastResponseContains("RDY")) {
            ESP_LOGI(TAG, "SIM7600 inició correctamente.");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));  // Esperar 1 segundo
    }

    int retries = 10;  // Intentar 10 veces enviar "AT"
    while (retries--) {
        ESP_LOGI(TAG, "Intento %d: Enviando AT...", 10 - retries);
        serialConsole::getInstance().sendCommand("AT");
        vTaskDelay(pdMS_TO_TICKS(1000));  // Esperar respuesta 1 segundo

        if (uartManager::getInstance().lastResponseContains("OK")) {
            ESP_LOGI(TAG, "SIM7600 está listo para recibir comandos.");
            return true;
        }
    }

    ESP_LOGE(TAG, "ERROR: SIM7600 no respondió a AT. Verifica la alimentación y conexión.");
    return false;
}
bool SIM7600::checkAndReconnectTCP() {
    serialConsole::getInstance().sendCommand("AT+CIPOPEN?");
    vTaskDelay(pdMS_TO_TICKS(1000));

    if (!uartManager::getInstance().lastResponseContains("34.196.135.179")) {
        ESP_LOGW(TAG, "Conexión TCP perdida, intentando reconectar...");
        serialConsole::getInstance().sendCommand("AT+CIPOPEN=0,\"TCP\",\"34.196.135.179\",5200");
        vTaskDelay(pdMS_TO_TICKS(3000));

        if (uartManager::getInstance().lastResponseContains("+CIPOPEN: 0,0")) {
            ESP_LOGI(TAG, "Conexión TCP reestablecida.");
            return true;
        } else {
            ESP_LOGE(TAG, "No se pudo reconectar con el servidor.");
            return false;
        }
    } else {
        ESP_LOGW(TAG, "EL servidor TCP ya esta en la lista de AT+CIPOPEN?");
        return true;
    }
}
void SIM7600::configureSIM7600() {
    if (!isModuleReady()) return;

    const char* commands[] = {
        "AT+CGPS=1",
        "AT+CGPSINFO=29",
        "AT+CPSI=28",
        "AT+NETOPEN"
    };

    for (const char* cmd : commands) {
        serialConsole::getInstance().sendCommand(cmd);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    int retries = 5;
    while (retries--) {
        serialConsole::getInstance().sendCommand("AT+CIPOPEN=0,\"TCP\",\"34.196.135.179\",5200");
        vTaskDelay(pdMS_TO_TICKS(2000));

        if (uartManager::getInstance().lastResponseContains("+CIPOPEN: 0,0")) {
            ESP_LOGI(TAG, "Conexión TCP establecida con éxito.");
            return;
        }

        ESP_LOGW(TAG, "Falló la conexión TCP. Reintentando...");
    }

    ESP_LOGE(TAG, "ERROR: No se pudo conectar al servidor TCP.");
}

void SIM7600::sendTCPMessage() {
    ESP_LOGW(TAG, "Iniciando sendTCPMessage ~~~~~~~~~~~~~~~~~~~~~>");
    if (!waitingForPrompt) {
        waitingForPrompt = true;
        std::string command = "AT+CIPSEND=0," + std::to_string(message.length());
        ESP_LOGI(TAG, "enviando comando =>%s",command.c_str());
        serialConsole::getInstance().sendCommand(command);
    }else {
        ESP_LOGI(TAG, "El mensaje no se envió validando conexión TCP...");
        if(checkAndReconnectTCP()) {
            waitingForPrompt = false;
        }  
    }
}
void SIM7600::processUARTEvent(std::string line) {
    if (waitingForPrompt && line.find(">") != std::string::npos) {
        ESP_LOGI(TAG, "📡 Indicación (>) para mandar msj a servidor recivida");
        waitingForPrompt = false;
        serialConsole::getInstance().sendCommand(message);
    }else if(line.find("+CGPSINFO:") != std::string::npos) {
        gpsReportReady = true;
        //parseData::getInstance().GPS(line);
        parseData::getInstance().GPS(line);
        
        if (tkr.fix == 0) { 
            ESP_LOGW(TAG, "No hay fix GNSS, solicitando AT+CCLK...");
            serialConsole::getInstance().sendCommand("AT+CCLK?");
        } else {
            updateMessage();
        }
        if (gpsReportReady && psiReportReady) {
            sendTCPMessage();
            gpsReportReady = false;
        }
    }else if (line.find("+CPSI:") != std::string::npos) {
        ESP_LOGI(TAG, "Evento CPSI recibido#%s",line.c_str());
        parseData::getInstance().PSI(line);
        psiReportReady = true;
        updateMessage();

    }else if(line.find("+CCLK:") != std::string::npos) {
        ESP_LOGI(TAG, "datos realtime leidos y listo para parsear");
        parseData::getInstance().CLK(line);
        updateMessage();

    }else if (line.find("+CMTI:") != std::string::npos) {
        ESP_LOGI(TAG, "SMS Detectado enviando comando para leer...");
        std::string index = line.substr(line.find(",") + 1);
        std::string command = "AT+CMGR=" + index;
        serialConsole::getInstance().sendCommand(command);
        
    } else if(line.find("+CIPOPEN: 0,") != std::string::npos) {
        ESP_LOGI(TAG, "el cliente TCP ya está configurado %s", line.c_str());
        waitingForPrompt = false;

    }else if(line.find("+IPCLOSE: 0,") != std::string::npos) {//+IPCLOSE: 0,2 ERROR?
        ESP_LOGW(TAG, "La conexión TCP se cerró inesperadamente reconectando..."); //si es 0,2 la cadena no es la adecuada (no tiene header ni IMEI)
        checkAndReconnectTCP();
    }else if(line.find("+CIPERROR: 4") != std::string::npos) {
        ESP_LOGW(TAG,  "Network is already opened, no necesitas reabrir la conexión​");
        /*serialConsole::getInstance().sendCommand("AT+CIPCLOSE=0");  
        vTaskDelay(pdMS_TO_TICKS(2000));*/
        checkAndReconnectTCP();
    }else if(line.find("+CIPSEND: 0,") != std::string::npos) {
        ESP_LOGI(TAG, "Mensaje enviado exitosamente, reiniciando envío en 30s.");
        waitingForPrompt = false;
    }
}