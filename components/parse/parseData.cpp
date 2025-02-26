#include "parseData.h"

static const char *TAG = "PARSE_DATA";

parseData& parseData::getInstance() {
    static parseData instance;
    return instance;
}

void parseData::GPS(const std::string& response) {
    std::string cleanResponse = utils::getInstance().cleanData(response, "CGNSSINFO");
    
    ESP_LOGI(TAG, "Clean CGNSSINFO =>%s", cleanResponse.c_str());
    
    if (cleanResponse.find(",,,,,,,,,,,,,,,") != std::string::npos) {
        ESP_LOGW(TAG, "No hay fix GNSS.");
        tkr.fix = 0;
        return;
    }
    std::vector<std::string> tokens = utils::getInstance().splitString(cleanResponse, ',');
    // Validar que hay suficientes datos
    if (tokens.size() < 16) {
        ESP_LOGW(TAG, "Datos insuficientes en GNSS,  manteniendo valores por defecto.");
        tkr.fix = 0;
        return;
    }

    // Validar que los valores no estén vacíos antes de convertirlos
    tkr.gps_svs = std::stoi(tokens[1]);
    // tkr.glonass_svs = std::stoi(tokens[2]);
    // tkr.beidou_svs = std::stoi(tokens[3]);
    tkr.lat = tokens[4];
    tkr.lon = tokens[6];
    tkr.date = tokens[8];
    tkr.time = tokens[9];
    tkr.speed = std::stod(tokens[11]);
    tkr.course = std::stod(tokens[12]);

    tkr.fix = 1; // Solo marcamos fix si hay datos válidos

    ESP_LOGI(TAG, "GNSS Parseado: Fecha:%s Hora:%s Lat:%s Lon:%s Velocidad:%.2f Curso:%.2f", 
             tkr.date.c_str(), tkr.time.c_str(), tkr.lat.c_str(), 
             tkr.lon.c_str(), tkr.speed, tkr.course);
}

void parseData::PSI(const std::string& response) {
    std::string cleanResponse = utils::getInstance().cleanData(response, "CPSI");
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
    
    std::string cleanResponse = utils::getInstance().cleanData(response, "CCLK");
    ESP_LOGI(TAG, "parseCCLK =>%s", cleanResponse.c_str());

    size_t start = cleanResponse.find("\"") + 1;
    size_t end = cleanResponse.find("\"", start);
    std::string datetime = cleanResponse.substr(start, end - start);
    
    convertToUTC(datetime);
}
void parseData::convertToUTC(const std::string& datetime) {
    if (datetime.empty()) {
        ESP_LOGE(TAG, "Error: La cadena de fecha/hora está vacía.");
        return;
    }
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

    ESP_LOGI(TAG, "Hora UTC Parseada: %s %s", tkr.date.c_str(), tkr.time.c_str());
}

/*void parseData::GPS(const std::string& response) {
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
}*/
 