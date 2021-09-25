#pragma once
#include <display.h>

namespace bk {
class ActivitySplashView {
 public:
    ActivitySplashView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
};
}  // namespace bk
