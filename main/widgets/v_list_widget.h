#pragma once
#include <vector>
#include "display.h"

namespace bk {
class VListWidget {
 public:
    VListWidget(IDisplay *display,
      const sFONT &font,
      const Rect &area);

    // Draw static components not changing over time
    void drawStatic();

    void goDown();
    void goUp();

    std::string getCurrentSelection();
    size_t getCurrentSelectionIdx();

    void updateElements(const std::vector<std::string> elements);

 protected:
    void drawNotSelected(size_t selection);
    void drawSelected(size_t selection);

    IDisplay *display_;
    std::vector<std::string> elements_;
    sFONT font_;
    Rect area_;
    // Height in pixels of single list element on the display
    int el_height_;
    int el_width_;
    // Offset to add to center the text inside list element in vertical dim
    int center_vert_;
    size_t current_selection_;

};
}  // namespace bk
