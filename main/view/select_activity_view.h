#pragma once
#include <vector>
#include "display.h"
#include "v_list_view.h"

namespace bk {
class SelectActivityView {
 public:
    SelectActivityView(IDisplay *display, VListView *activity_list);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
    std::vector<std::string> elements_;
};
}  // namespace bk
