#include "stats_view.h"

#include <paint.h>
namespace bk {
;

StatsView::StatsView(IDisplay *display)
    : display_(display),
      activities_(VListWidget(display, Font16, {149, 13, 295, 127})) {
}

void StatsView::drawStatic() {
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

            snprintf(message, msg_size, "Stats");
            paint.DrawStringAt(31, 29, message, &Font24, COLORED);
        },
        {1, 13, 147, 69});

    // Will potentially overlay above statements
    activities_.drawStatic();
}

VListWidget &StatsView::getActivities() {
    return activities_;
}
}  // namespace bk
