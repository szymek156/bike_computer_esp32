#pragma once
#include "display.h"

#include <vector>

namespace bk {
enum class Alignment { Left, Centered };

class VListWidget {
 public:
    VListWidget(IDisplay *display,
                const sFONT &font,
                const Rect &area,
                size_t el_in_view = 4,
                Alignment Alignment = Alignment::Centered);

    // Draw static components not changing over time
    void drawStatic();

    void goDown();
    void goUp();

    void clearSelection();

    std::string getCurrentSelection();
    size_t getCurrentSelectionIdx();

    void updateElements(const std::vector<std::string> elements);

 protected:
    void invalidateViewport();
    void drawNotSelected(size_t selection);
    void drawSelected(size_t selection);
    int computeAlignment(size_t selection);

    IDisplay *display_;
    std::vector<std::string> elements_;
    sFONT font_;
    Rect area_;

    // Offset to add to center the text inside list element in vertical dim
    int center_vert_;
    size_t current_selection_;

    // Number of elements visible at once set by user
    size_t el_in_view_;
    size_t viewport_start_;
    // Number of elements visible at once, it's a min(el_in_view_, elements_.size())
    size_t viewport_size_;

    // Height in pixels of single list element on the display
    int el_height_;
    int el_width_;

    Alignment alignment_;
};
}  // namespace bk
