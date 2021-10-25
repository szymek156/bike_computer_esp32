#pragma once
#include <display.h>

namespace bk {
class Running1View {
 public:
    Running1View(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
};
}  // namespace bk
