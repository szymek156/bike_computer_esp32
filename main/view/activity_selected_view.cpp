#include "activity_selected_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

ActivitySelectedView::ActivitySelectedView(IDisplay *display)
    : display_(display),
      options_(VListWidget(display, {"Do It", "View"}, Font16, {149, 13, 295, 127})) {
}

void ActivitySelectedView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(13, 12, 270, COLORED);
            paint.DrawVerticalLine(148, 15, 110, COLORED);
            paint.DrawHorizontalLine(13, 70, 122, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Activity type");
            paint.DrawStringAt(14, 29, message, &Font16, COLORED);
            snprintf(message, msg_size, "Workout type");
            paint.DrawStringAt(13, 91, message, &Font16, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});

        options_.drawStatic();
}

VListWidget &ActivitySelectedView::getOptions() {
    return options_;
}
}  // namespace bk
