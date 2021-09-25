#pragma once
#include <display.h>

namespace bk {
class TestView {
 public:
    TestView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();
    void drawWeatherData(const WeatherData &data);
    void drawDateTime(const tm &time_info);
    void drawGNSSData(const GNSSData &data);

 protected:
    IDisplay *display_;
};
}  // namespace bk
