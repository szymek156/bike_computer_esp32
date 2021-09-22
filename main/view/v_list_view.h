#pragma once
#include <vector>
#include "display.h"

namespace bk {
class VListView {
 public:
    VListView(IDisplay *display, const std::vector<std::string> &elements);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
    std::vector<std::string> elements_;
};
}  // namespace bk
