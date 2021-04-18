#pragma once
#include "display.h"

#include <epdpaint.h>

namespace bk {
class StatusView {
 public:
    StatusView(IDisplay *display);
    void drawGNSSData(const GNSSData &data);
    void drawDateTime(const GNSSData &data);

 protected:
    IDisplay *display_;
    Paint paint_;
};
}  // namespace bk
