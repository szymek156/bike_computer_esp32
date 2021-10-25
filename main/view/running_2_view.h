#pragma once
#include <display.h>

namespace bk {
class Running2View {
 public:
    Running2View(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
};
}  // namespace bk
