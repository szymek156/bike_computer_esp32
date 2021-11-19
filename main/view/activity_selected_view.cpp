#include "activity_selected_view.h"

#include <paint.h>
namespace bk {
;

ActivitySelectedView::ActivitySelectedView(IDisplay *display)
    : display_(display),
      options_(VListWidget(display, Font16, {149, 13, 295, 127})),
      activity_("Empty activity"),
      workout_("Empty workout") {
}

void ActivitySelectedView::drawStatic() {
    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            paint.DrawHorizontalLine(13, 12, 270, COLORED);
            paint.DrawVerticalLine(148, 15, 110, COLORED);
            paint.DrawHorizontalLine(13, 70, 122, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, activity_.c_str());
            paint.DrawStringAt(14, 29, message, &Font24, COLORED);
            snprintf(message, msg_size, workout_.c_str());
            paint.DrawStringAt(13, 91, message, &Font24, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});

        options_.drawStatic();
}

VListWidget &ActivitySelectedView::getOptions() {
    return options_;
}
}  // namespace bk
