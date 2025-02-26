#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <sstream>
#include "esp_log.h"
#include "trackerData.h"

class utils {
    private:
    utils() = default; // Constructor privado para evitar instancias múltiples
    std::string removeHexPrefix(const std::string& hexValue);
    public:
        static utils& getInstance(); // Singleton
        std::vector<std::string> splitString(const std::string& str, char delimiter);
        std::string cleanData(const std::string& response, const std::string& command);

        void parseGSM(const std::vector<std::string>& tokens);
        void parseLTE(const std::vector<std::string>& tokens);
        void parseWCDMA(const std::vector<std::string>& tokens);
        void parseCDMA(const std::vector<std::string>& tokens);
        void parseEVDO(const std::vector<std::string>& tokens);
};
#endif