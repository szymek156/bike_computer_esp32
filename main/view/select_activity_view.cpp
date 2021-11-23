#include "select_activity_view.h"

#include <paint.h>
namespace bk {
;

SelectActivityView::SelectActivityView(IDisplay *display)
    : display_(display),
      activities_(VListWidget(display, Font24, {201, 25, 399, 239})) {
}

void SelectActivityView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawVerticalLine(200, 27, 210, COLORED);
            paint.DrawHorizontalLine(13, 132, 174, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});

    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Activity");
            paint.DrawStringAt(4, 57, message, &Font42, COLORED);
        },
        {1, 25, 199, 131});

    // Will potentially overlay above statements
    activities_.drawStatic();
}

VListWidget &SelectActivityView::getActivities() {
    return activities_;
}

}  // namespace bk
