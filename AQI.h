#include <vector>
#include <string>
#include <unordered_map>

#include <d2d1.h>

// #define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

struct timestamp {
    int year, month, day, hour;
};

struct AQIData {
    timestamp time;
    int pm25;
};

class AQI {
private:
    char* url;
public:
    std::vector<AQIData> aqiData;
    AQI(const char* url);
    ~AQI() {}

    bool fetch_24_hours_data();
    int get_latest_pm25();

    D2D1::ColorF get_pm25_color(int pm25);
};