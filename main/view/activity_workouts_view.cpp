#include "activity_workouts_view.h"

#include <paint.h>
namespace bk {
using bk::COLORED;

ActivityWorkoutsView::ActivityWorkoutsView(IDisplay *display)
    : display_(display),
      workouts_(VListWidget(display, Font16, {149, 13, 295, 127})) {
}

void ActivityWorkoutsView::drawStatic() {
    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            paint.DrawHorizontalLine(13, 12, 270, COLORED);
            paint.DrawVerticalLine(148, 15, 110, COLORED);
            paint.DrawHorizontalLine(13, 70, 122, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Workouts");
            paint.DrawStringAt(6, 29, message, &Font24, COLORED);

            snprintf(message, msg_size, activity_type_.c_str());
            paint.DrawStringAt(14, 87, message, &Font24, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});

    workouts_.drawStatic();
}

VListWidget &ActivityWorkoutsView::getWorkouts() {
    return workouts_;
}

}  // namespace bk
