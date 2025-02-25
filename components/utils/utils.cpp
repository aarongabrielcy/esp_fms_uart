#include "utils.h"

static const char *TAG = "UTILS";

utils& utils::getInstance() {
    static utils instance;
    return instance;
}

//Divide una cadena en tokens usando un delimitador
std::vector<std::string> utils::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

//Procesa datos de GSM
void utils::parseGSM(const std::vector<std::string>& tokens) {
    if (tokens.size() < 9) {
        ESP_LOGE(TAG, "Error: Datos insuficientes en GSM.");
        return;
    }

    tkr.mcc = std::stoi(tokens[2].substr(0, 3));
    tkr.mnc = std::stoi(tokens[2].substr(4, 2));
    tkr.lac_tac = tokens[3];
    tkr.cell_id = tokens[4];
    tkr.rxlvl_rsrp = std::stoi(tokens[6]);

    ESP_LOGI(TAG, "GSM Parseado: MCC:%d, MNC:%d, LAC:%s, CellID:%s, RXLVL:%d",
             tkr.mcc, tkr.mnc, tkr.lac_tac.c_str(), tkr.cell_id.c_str(), tkr.rxlvl_rsrp);
}

//Procesa datos de LTE
void utils::parseLTE(const std::vector<std::string>& tokens) {
    if (tokens.size() < 14) {
        ESP_LOGE(TAG, "Error: Datos insuficientes en LTE.");
        return;
    }

    tkr.mcc = std::stoi(tokens[2].substr(0, 3));
    tkr.mnc = std::stoi(tokens[2].substr(4, 3));
    tkr.lac_tac = tokens[3];
    tkr.cell_id = tokens[4];
    tkr.rxlvl_rsrp = std::stoi(tokens[11]);

    ESP_LOGI(TAG, "LTE Parseado: MCC:%d, MNC:%d, TAC:%s, CellID:%s, RSRP:%d",
             tkr.mcc, tkr.mnc, tkr.lac_tac.c_str(), tkr.cell_id.c_str(), tkr.rxlvl_rsrp);
}

//Procesa datos de WCDMA
void utils::parseWCDMA(const std::vector<std::string>& tokens) {
    if (tokens.size() < 14) {
        ESP_LOGE(TAG, "Error: Datos insuficientes en WCDMA.");
        return;
    }

    tkr.mcc = std::stoi(tokens[2].substr(0, 3));
    tkr.mnc = std::stoi(tokens[2].substr(4, 2));
    tkr.lac_tac = tokens[3];
    tkr.cell_id = tokens[4];
    tkr.rxlvl_rsrp = std::stoi(tokens[12]);

    ESP_LOGI(TAG, "WCDMA Parseado: MCC:%d, MNC:%d, LAC:%s, CellID:%s, RXLVL:%d",
             tkr.mcc, tkr.mnc, tkr.lac_tac.c_str(), tkr.cell_id.c_str(), tkr.rxlvl_rsrp);
}

// Procesa datos de CDMA
void utils::parseCDMA(const std::vector<std::string>& tokens) {
    if (tokens.size() < 14) {
        ESP_LOGE(TAG, "Error: Datos insuficientes en CDMA.");
        return;
    }

    tkr.mcc = std::stoi(tokens[2].substr(0, 3));
    tkr.mnc = std::stoi(tokens[2].substr(4, 2));
    tkr.rxlvl_rsrp = std::stoi(tokens[6]);

    ESP_LOGI(TAG, "CDMA Parseado: MCC:%d, MNC:%d, RXLVL:%d",
             tkr.mcc, tkr.mnc, tkr.rxlvl_rsrp);
}

// Procesa datos de EVDO
void utils::parseEVDO(const std::vector<std::string>& tokens) {
    if (tokens.size() < 10) {
        ESP_LOGE(TAG, "Error: Datos insuficientes en EVDO.");
        return;
    }

    tkr.mcc = std::stoi(tokens[2].substr(0, 3));
    tkr.mnc = std::stoi(tokens[2].substr(4, 2));
    tkr.rxlvl_rsrp = std::stoi(tokens[5]);

    ESP_LOGI(TAG, "EVDO Parseado: MCC:%d, MNC:%d, RXLVL:%d",
             tkr.mcc, tkr.mnc, tkr.rxlvl_rsrp);
}
