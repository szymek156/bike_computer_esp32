#pragma once
#include "widgets/v_list_widget.h"

#include <display.h>
namespace bk {
class StatsView {
 public:
    StatsView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();
    VListWidget &getActivities();

 protected:
    IDisplay *display_;
    VListWidget activities_;

};
}  // namespace bk
