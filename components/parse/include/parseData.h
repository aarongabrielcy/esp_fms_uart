#ifndef PARSEDATA_H
#define PARSEDATA_H

#include <string>
#include <vector>
#include <sstream>
#include "esp_log.h"
#include <iomanip>
#include "utils.h"
#include "trackerData.h" 
#include "SIM7600.h"

class parseData {
    private:
        parseData() = default;
    public:
        static parseData& getInstance();
        void GPS(const std::string& response);
        void PSI(const std::string& response);
        void CLK(const std::string& response);
        void convertToUTC(const std::string& datetime);
};
#endif

