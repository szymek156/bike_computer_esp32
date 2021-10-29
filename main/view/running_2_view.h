#pragma once
#include <display.h>

namespace bk {
class Running2View {
 public:
    Running2View(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

    void drawTotalDistance(float total_distance);
    void drawLapDistance(float lap_distance);
    void drawLapTime(int hours, int minutes, int seconds);
    void drawTotalTime(int hours, int minutes, int seconds);

 protected:
    IDisplay *display_;
};
}  // namespace bk
