#pragma once
#include <display.h>

namespace bk {
class WelcomeView {
 public:
    WelcomeView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();
    void drawDateTime(const tm &time_info);

 protected:
    IDisplay *display_;
};
}  // namespace bk
