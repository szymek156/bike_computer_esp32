#pragma once
#include <display.h>
#include <vector>
#include <string>
#include "widgets/v_list_widget.h"
namespace bk {
class ActivityWorkoutsView {
 public:
    ActivityWorkoutsView(IDisplay *display, const std::vector<std::string> &workouts);

    // Draw static components not changing over time
    void drawStatic();

    VListWidget &getWorkouts();

 protected:
    IDisplay *display_;
    VListWidget workouts_;
};
}  // namespace bk
