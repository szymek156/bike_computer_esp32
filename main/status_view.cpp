#include "status_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

StatusView::StatusView(IDisplay *display) : display_(display) {
}

void StatusView::drawStatic() {
    display_->enqueueDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(0, 13, paint.GetWidth() * 8, COLORED);
        },
        {0, 0, 0, 0});
}

void StatusView::drawGNSSData(const GNSSData &data) {
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "GPS %1d", data.fix_status);
            paint.DrawStringAt(180, 4, message, &Font8, COLORED);
        },
        {180, 4, 210, 10});
}

void StatusView::drawDateTime(const tm &time_info) {
    display_->enqueueDraw(
        [&time_info](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%D %T", &(time_info));
            paint.DrawStringAt(0, 4, message, &Font8, COLORED);
        },
        {0, 4, 100, 10});
}
}  // namespace bk
