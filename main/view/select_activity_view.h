#pragma once
#include "display.h"

namespace bk {
class SelectActivityView {
 public:
    SelectActivityView(IDisplay *display, std::vector<std::string> elements);
    
    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
    std::vector<std::string> elements_;
};
}  // namespace bk
