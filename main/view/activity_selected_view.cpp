#include "activity_selected_view.h"

#include <paint.h>
namespace bk {
;

ActivitySelectedView::ActivitySelectedView(IDisplay *display)
    : display_(display),
      options_(VListWidget(display, Font24, {201, 25, 399, 239})),
      activity_("Empty activity"),
      workout_("Empty workout") {
}

void ActivitySelectedView::drawStatic() {
    display_->enqueueDraw(
        [&](Paint &paint) {
            paint.DrawVerticalLine(200, 27, 210, COLORED);
            paint.DrawHorizontalLine(13, 132, 174, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, activity_.c_str());
            paint.DrawStringAt(16, 57, message, &Font42, COLORED);
            snprintf(message, msg_size, workout_.c_str());
            paint.DrawStringAt(1, 170, message, &Font31, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});

    options_.drawStatic();
}

VListWidget &ActivitySelectedView::getOptions() {
    return options_;
}
}  // namespace bk
