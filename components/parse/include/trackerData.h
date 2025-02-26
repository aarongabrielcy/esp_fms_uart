
#ifndef TRACKERDATA_H
#define TRACKERDATA_H

#include <string>

struct trackerData {
    std::string header = "STT";
    std::string imei = "2049830928";
    std::string rep_map = "3FFFFF";
    int model = 99;
    std::string sw_ver = "1.0.1";
    int msg_type = 1;
    std::string date = "00000000";
    std::string time = "00:00:00";
    std::string cell_id = "00000000";
    int mcc = 0;
    int mnc = 0;
    std::string lac_tac = "FFFF";
    int rxlvl_rsrp = 999;
    std::string lat = "+00.000000";
    std::string lon = "-00.000000";
    float speed = 0.00;
    float course = 0.00;
    int gps_svs = 0;
    int fix = 0;
    std::string in_state = "00000100";
    std::string out_state = "00001000";
    int mode = 0;
    int stt_rpt_type = 0;
    int msg_num = 0;
    float bck_volt = 0.00;
    float power_Volt = 0.00;
};
inline trackerData tkr;
#endif