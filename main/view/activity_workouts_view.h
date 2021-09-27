#pragma once
#include "widgets/v_list_widget.h"

#include <string>
#include <vector>

#include <display.h>
namespace bk {
class ActivityWorkoutsView {
 public:
    ActivityWorkoutsView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

    VListWidget &getWorkouts();

    void setActivity(const std::string &activity) {
        activity_type_ = activity;
    }

 protected:
    IDisplay *display_;
    VListWidget workouts_;
    std::string activity_type_;
};
}  // namespace bk
