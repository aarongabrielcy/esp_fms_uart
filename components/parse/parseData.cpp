#include "parseData.h"

static const char *TAG = "PARSE_DATA";

parseData& parseData::getInstance() {
    static parseData instance;
    return instance;
}

void parseData::GPS(const std::string& response) {
    std::string cleanResponse = response;
    size_t prefixPos = cleanResponse.find("+CGNSSINFO: ");
    if (prefixPos != std::string::npos) {
        cleanResponse = cleanResponse.substr(prefixPos + 7); // Saltar "+CPSI: "
    }
    // Dividir la cadena en tokens
    std::vector<std::string> tokens = utils::getInstance().splitString(cleanResponse, ',');

    // Validar si hay suficientes datos
    if (tokens.size() < 16 || cleanResponse.find_first_not_of(",") == std::string::npos) {
        tkr.fix = 0;
        ESP_LOGW(TAG, "No hay fix GNSS, asignando valores por defecto.");
        return;
    }

    //Asignación de datos parseados
    //mode = std::stoi(tokens[0]);               // Modo de GNSS
    tkr.gps_svs = std::stoi(tokens[1]);            // Satélites GPS en uso
    //glonass_svs = std::stoi(tokens[2]);        // Satélites GLONASS en uso
    //beidou_svs = std::stoi(tokens[3]);         // Satélites BEIDOU en uso
    tkr.lat = tokens[4];                           // Latitud
    tkr.lon = tokens[6];                           // Longitud
    tkr.date = tokens[8];                          // Fecha UTC en formato DDMMYY
    tkr.time = tokens[9];                          // Hora UTC en formato HHMMSS.s
    tkr.speed = std::stod(tokens[11]);             // Velocidad en nudos
    tkr.course = std::stod(tokens[12]);            // Dirección en grados
    tkr.fix = 1;                                   // Indica que hay un fix válido

    /*ESP_LOGI(TAG, "📡 GNSS Parseado: Fecha:%s Hora:%s Lat:%s Lon:%s Velocidad:%.2f Curso:%.2f", 
             tkr.date.c_str(), tkr.time.c_str(), tkr.lat.c_str(), 
             tkr.lon.c_str(), tkr.speed, tkr.course);*/
}

void parseData::PSI(const std::string& response) {
    // Eliminar el prefijo "+CPSI: " si está presente vuelve esto una funcion en utils.cpp
    std::string cleanResponse = response;
    size_t prefixPos = cleanResponse.find("+CPSI: ");
    if (prefixPos != std::string::npos) {
        cleanResponse = cleanResponse.substr(prefixPos + 7); // Saltar "+CPSI: "
    }
    std::vector<std::string> tokens = utils::getInstance().splitString(cleanResponse, ',');
    if (tokens.empty()) {
        ESP_LOGE(TAG, "Error: String vacío en PSI.");
        return;
    }
    std::string systemMode = tokens[0];

    if (systemMode == "GSM") {
        utils::getInstance().parseGSM(tokens);
    } else if (systemMode == "LTE") {
        utils::getInstance().parseLTE(tokens);
    } else if (systemMode == "WCDMA") {
        utils::getInstance().parseWCDMA(tokens);
    } else if (systemMode == "CDMA") {
        utils::getInstance().parseCDMA(tokens);
    } else if (systemMode == "EVDO") {
        utils::getInstance().parseEVDO(tokens);
    } else {
        ESP_LOGW(TAG, "NO SERVICE");
    }
}

void parseData::CLK(const std::string& response) {
    ESP_LOGI(TAG, "parseCCLK =>%s", response.c_str());

    size_t start = response.find("\"") + 1;
    size_t end = response.find("\"", start);
    std::string datetime = response.substr(start, end - start);
    
    convertToUTC(datetime);
}
void parseData::convertToUTC(const std::string& datetime) {
    std::istringstream ss(datetime);
    int year, month, day, hour, minute, second, timezone_quarters;

    char sep;  // Para leer los separadores "/,:,"
    ss >> year >> sep >> month >> sep >> day >> sep >> hour >> sep >> minute >> sep >> second >> timezone_quarters;

    if (ss.fail()) {
        ESP_LOGE(TAG, "Error al parsear la fecha y hora.");
        return;
    }

    // Convertimos a UTC
    int timezone_offset = timezone_quarters / 4;  // Convertir cuartos de hora a horas completas
    hour -= timezone_offset;

    // Ajustamos desbordamiento de horas
    if (hour < 0) {
        hour += 24;
        day--;
    } else if (hour >= 24) {
        hour -= 24;
        day++;
    }

    //Formateamos la fecha a `YYYYMMDD`
    std::ostringstream dateStream;
    dateStream << "20" << std::setw(2) << std::setfill('0') << year  // "25" → "2025"
               << std::setw(2) << month
               << std::setw(2) << day;
    tkr.date = dateStream.str();

    //Formateamos la hora a `HH:MM:SS`
    std::ostringstream timeStream;
    timeStream << std::setw(2) << std::setfill('0') << hour << ":"
               << std::setw(2) << minute << ":"
               << std::setw(2) << second;
    tkr.time = timeStream.str();

    //ESP_LOGI(TAG, "Hora UTC Parseada: %s %s", tkr.date.c_str(), tkr.time.c_str());
}

/**
 * void parseData::GPS(const std::string& response) {
    ESP_LOGI(TAG, "parseCGPS =>%s", response.c_str());
    if (response.find(" ,,,,,,,,,,,,,,") != std::string::npos) {
        ESP_LOGW(TAG, "No hay fix GNSS.");
        tkr.fix = 0;
        return;
    }
    tkr.date = "20250224";
    tkr.time = "15:36:27";
    tkr.lat = "+21.023739";
    tkr.lon = "-89.584597";
    tkr.speed = 0.19;
    tkr.course = 0.36;
    tkr.fix = 0;

    ESP_LOGI(TAG, "📡 GNSS Parseado: %s %s %s %s %.2f %.2f", 
             tkr.date.c_str(), tkr.time.c_str(), tkr.lat.c_str(), 
             tkr.lon.c_str(), tkr.speed, tkr.course);
}
 */