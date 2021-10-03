#pragma once
#include <string>

#include <display.h>
namespace bk {
class ActivityDoItView {
 public:
    ActivityDoItView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

    void drawGNSSData(const GNSSData &data);

    void setActivity(const std::string &activity) {
        activity_ = activity;
    }

    void setWorkout(const std::string &workout) {
        workout_ = workout;
    }

 protected:
    IDisplay *display_;

    std::string activity_;
    std::string workout_;
};
}  // namespace bk
