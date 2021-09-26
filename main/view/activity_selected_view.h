#pragma once
#include "widgets/v_list_widget.h"

#include <display.h>
namespace bk {
class ActivitySelectedView {
 public:
    ActivitySelectedView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();
    VListWidget &getOptions();

 protected:
    IDisplay *display_;
    VListWidget options_;
};
}  // namespace bk
