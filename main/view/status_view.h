#pragma once
#include "display.h"

namespace bk {
class StatusView {
 public:
    StatusView(IDisplay *display);
    
    // Draw static components not changing over time
    void drawStatic();
    void drawGNSSData(const GNSSData &data);
    void drawDateTime(const tm &time_info);

 protected:
    IDisplay *display_;
};
}  // namespace bk
