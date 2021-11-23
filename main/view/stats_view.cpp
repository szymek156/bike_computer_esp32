#include "stats_view.h"

#include <paint.h>
namespace bk {
;

StatsView::StatsView(IDisplay *display)
    : display_(display),
      activities_(VListWidget(display, Font24, {201, 25, 399, 239})) {
}

void StatsView::drawStatic() {
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

            snprintf(message, msg_size, "Stats");
            paint.DrawStringAt(40, 57, message, &Font42, COLORED);
        },
        {1, 25, 199, 131});

    // Will potentially overlay above statements
    activities_.drawStatic();
}

VListWidget &StatsView::getActivities() {
    return activities_;
}
}  // namespace bk
