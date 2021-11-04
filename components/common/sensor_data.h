#pragma once
#include <ctime>
#include <memory>
namespace bk {

class PagePresenter;

struct WidgetData {
    enum Widget { next, prev, more, less };

    Widget new_widget;
    // There are cases, where can be many "more" widgets
    // this fields specifies exactly which one is it
    size_t widget_idx = 0;
};

struct TimeData {};

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

    // TODO: and others
    GNSSData() : fix_status(noFix) {
    }

    /** @brief A semicircle is an unit of location-based measurement on an arc.
     * An arc of 180 degrees is made up of many semicircle units; 2^31 semicircles to be exact.
     * Semicircles that correspond to North latitudes and East longitudes are indicated with
     * positive values; semicircles that correspond to South latitudes and West longitudes are
     * indicated with negative values.
     *
     * The following formulas show how to convert between degrees and semicircles:
     * degrees = semicircles * (180 / 2^31)
     * semicircles = degrees * (2^31 / 180)
     */
    static int32_t toSemiCircles(float degrees) {
        static constexpr auto convert = double(1 << 31) / 180.0;
        return degrees * convert;
    }

    static float toDegrees(int32_t semicircles) {
        static constexpr auto convert = 180.0 / double(1 << 31);
        return semicircles * convert;
    }

    std::tm date_time;

    float latitude;
    float longitude;
    float altitude;

    float speed_ms;
    float track_degrees;

    FixStatus fix_status;
    FixQuality fix_quality;

    int sats_in_view;
    int sats_tracked;
};

struct Time {
    int hours;
    int minutes;
    int seconds;
};
struct ActivityData {
    int speed_ms;
    float lap_distance;
    float total_distance;
    // hr_zone
    // cadence
    // stride?
    Time lap_time;
    Time total_time;
    // TODO: more to come...
};

}  // namespace bk