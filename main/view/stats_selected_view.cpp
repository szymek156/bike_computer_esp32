#include "stats_selected_view.h"

#include <paint.h>
namespace bk {
;

StatsSelectedView::StatsSelectedView(IDisplay *display) : display_(display) {
}

void StatsSelectedView::drawStatic() {
    display_->enqueueDraw(
        [](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            paint.DrawHorizontalLine(13, 67, 374, COLORED);
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Stats for: workout type");
            paint.DrawStringAt(39, 33, message, &Font24, COLORED);
            snprintf(message, msg_size, "All time:");
            paint.DrawStringAt(137, 76, message, &Font24, COLORED);
            snprintf(message, msg_size, "5k: 50min");
            paint.DrawStringAt(137, 119, message, &Font24, COLORED);
            snprintf(message, msg_size, "10k: 4hrs");
            paint.DrawStringAt(137, 162, message, &Font24, COLORED);
            snprintf(message, msg_size, "Half M: 2 days 4hrs");
            paint.DrawStringAt(67, 205, message, &Font24, COLORED);
        });
}
}  // namespace bk
