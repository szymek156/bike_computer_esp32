#include "stats_view.h"

#include <paint.h>
namespace bk {
;

StatsView::StatsView(IDisplay *display)
    : display_(display),
      activities_(VListWidget(display, Font24, {201, 25, 399, 239})) {
}

void StatsView::drawStatic() {
    display_->enqueueDraw(
        [](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            paint.DrawVerticalLine(200, 27, 210, COLORED);
            paint.DrawHorizontalLine(13, 132, 174, COLORED);
        });

    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Stats");
            paint.DrawStringAt(40, 57, message, &Font42, COLORED);
        });

    // Will potentially overlay above statements
    activities_.drawStatic();
}

VListWidget &StatsView::getActivities() {
    return activities_;
}
}  // namespace bk
