#pragma once
#include <vector>
#include "display.h"
#include "widgets/v_list_widget.h"
namespace bk {
class SelectActivityView {
 public:
    SelectActivityView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

    VListWidget &getActivities();

 protected:
    IDisplay *display_;
    VListWidget activities_;

};
}  // namespace bk
