#include "activity_workouts_view.h"

#include <paint.h>
namespace bk {
;

ActivityWorkoutsView::ActivityWorkoutsView(IDisplay *display)
    : display_(display),
      workouts_(VListWidget(display, Font24, {201, 25, 399, 239})) {
}

void ActivityWorkoutsView::drawStatic() {
    display_->enqueueDraw(
        [&](Paint &paint) {
            paint.DrawVerticalLine(200, 27, 210, COLORED);
            paint.DrawHorizontalLine(13, 132, 174, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Workouts");
            paint.DrawStringAt(4, 57, message, &Font42, COLORED);

            snprintf(message, msg_size, activity_type_.c_str());
            paint.DrawStringAt(37, 170, message, &Font31, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});

    workouts_.drawStatic();
}

VListWidget &ActivityWorkoutsView::getWorkouts() {
    return workouts_;
}

}  // namespace bk
