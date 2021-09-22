#pragma once
#include <vector>
#include "display.h"

namespace bk {
class VListWidget {
 public:
    VListWidget(IDisplay *display,
      const std::vector<std::string> &elements,
      const Rect &area);

    // Draw static components not changing over time
    void drawStatic();

    void goDown();
    void goUp();

    std::string getCurrentSelection();
    size_t getCurrentSelectionIdx();

 protected:
    void clearSelection();
    void markSelection();

    IDisplay *display_;
    Rect area_;
    std::vector<std::string> elements_;
    // Height in pixels of single list element on the display
    int el_height_;
    size_t current_selection_;

};
}  // namespace bk
