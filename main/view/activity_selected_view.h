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

    void setActivity(const std::string &activity) {
        activity_ = activity;
    }

    void setWorkout(const std::string &workout) {
        workout_ = workout;
    }

 protected:
    IDisplay *display_;
    VListWidget options_;

    std::string activity_;
    std::string workout_;
};
}  // namespace bk
