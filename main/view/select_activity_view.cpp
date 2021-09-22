#include "select_activity_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

SelectActivityView::SelectActivityView(IDisplay *display, VListView *activity_list)
    : display_(display) {
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
}

}  // namespace bk
