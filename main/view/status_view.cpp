#include "status_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

StatusView::StatusView(IDisplay *display) : display_(display) {
}

void StatusView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawVerticalLine(98, 3, 6, COLORED);
            paint.DrawVerticalLine(196, 3, 6, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 0, display_->getWidth(), 13});
}

void StatusView::drawGNSSData(const GNSSData &data) {
    // GPS 3D
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "GPS %1d", data.fix_status);
            paint.DrawStringAt(132, 2, message, &Font8, COLORED);
        },
        {99, 1, 195, 11});
}

void StatusView::drawDateTime(const tm &time_info) {
    // 21:37
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%T", &(time_info));
            paint.DrawStringAt(36, 2, message, &Font8, COLORED);
        },
        {1, 1, 97, 11});

    // 02/09/21
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%d/%m/%y", &(time_info));
            paint.DrawStringAt(225, 2, message, &Font8, COLORED);
        },
        {197, 1, 293, 11});
}
}  // namespace bk
