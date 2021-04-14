#pragma once
#include <ctime>
namespace bk {
struct DisplayData {};

struct KeypadData {
    bool lu_pressed;
    bool ru_pressed;
    bool ld_pressed;
    bool rd_pressed;
};

struct WeatherData {
    float altitude_m;
    float temp_c;
};

struct GNSSData {
    enum FixStatus { noFix = 1, fix2d = 2, fix3d = 3 };

    enum FixQuality {
        error = 0,
        ideal = 1,
        excellent = 2,
        good = 5,
        moderate = 10,
        fair = 20,
        poor = 21
    };

    std::tm date_time;

    float latitude;
    float longitude;
    float altitude;

    float speed_kmh;
    float track_degrees;

    FixStatus fix_status;
    FixQuality fix_quality;

    int sats_in_view;
    int sats_tracked;
};

union SensorMessage {
    WeatherData weather;
    GNSSData gnss;
};

}  // namespace bk