#include "select_activity_view.h"

#include <paint.h>
namespace bk {
;

SelectActivityView::SelectActivityView(IDisplay *display)
    : display_(display),
    activities_(VListWidget(display, Font16
    , {149, 13, 295, 127})) {
}

void SelectActivityView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(13, 12, 270, COLORED);
            paint.DrawVerticalLine(148, 15, 110, COLORED);
            paint.DrawHorizontalLine(13, 70, 122, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});

    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Activity");
            paint.DrawStringAt(6, 29, message, &Font24, COLORED);
        },
        {1, 13, 147, 69});

    // Will potentially overlay above statements
    activities_.drawStatic();
}

VListWidget &SelectActivityView::getActivities() {
    return activities_;
}

}  // namespace bk
