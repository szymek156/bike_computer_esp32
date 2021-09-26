#include "stats_selected_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

StatsSelectedView::StatsSelectedView(IDisplay *display)
    : display_(display){
}

void StatsSelectedView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(13, 35, 270, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Stats for: workout type");
            paint.DrawStringAt(21, 15, message, &Font16, COLORED);
            snprintf(message, msg_size, "All time:");
            paint.DrawStringAt(116, 40, message, &Font12, COLORED);
            snprintf(message, msg_size, "5k: 50min");
            paint.DrawStringAt(116, 63, message, &Font12, COLORED);
            snprintf(message, msg_size, "10k: 4hrs");
            paint.DrawStringAt(116, 86, message, &Font12, COLORED);
            snprintf(message, msg_size, "Half M: 2 days 4hrs");
            paint.DrawStringAt(81, 109, message, &Font12, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});

}
}  // namespace bk
