#pragma once
#include <display.h>

namespace bk {
class Running1View {
 public:
    Running1View(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

    void drawCadence(int cadence);
    void drawStride(float stride);
    void drawHRZone(float zone);
    void drawPace(float pace);

 protected:
    IDisplay *display_;
};
}  // namespace bk
