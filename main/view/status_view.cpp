#include "status_view.h"

#include <paint.h>
namespace bk {
;

StatusView::StatusView(IDisplay *display) : display_(display) {
}

void StatusView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawVerticalLine(133, 3, 18, COLORED);
            paint.DrawVerticalLine(266, 3, 18, COLORED);
            paint.DrawHorizontalLine(13, 24, 374, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), 24});
}

void StatusView::drawGNSSData(const GNSSData &data) {
    static bool blink_status = false;

    // GPS 3D
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];
            snprintf(message, msg_size, "GPS %1d", data.fix_status);

            if (data.fix_status <= GNSSData::noFix) {
                paint.DrawFilledRectangle(134, 1, 265, 23, blink_status ? COLORED : UNCOLORED);

                paint.DrawStringAt(166, 2, message, &Font19, blink_status ? UNCOLORED : COLORED);
                blink_status = !blink_status;

            } else {
                if (blink_status) {
                    paint.DrawFilledRectangle(134, 1, 265, 23, UNCOLORED);
                    blink_status = false;
                }

                paint.DrawStringAt(166, 2, message, &Font19, COLORED);
            }
        },
        {134, 1, 265, 23});
}

void StatusView::drawDateTime(const tm &time_info) {
    // 21:37
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%T", &(time_info));
            paint.DrawStringAt(22, 2, message, &Font19, COLORED);
        },
        {1, 1, 132, 23});

    // 02/09/21
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%d/%m/%y", &(time_info));
            paint.DrawStringAt(288, 2, message, &Font19, COLORED);
        },
        {267, 1, 398, 23});
}
}  // namespace bk
