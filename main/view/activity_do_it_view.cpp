#include "activity_do_it_view.h"

#include "freertos/FreeRTOS.h"

#include <paint.h>
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>
namespace bk {
using bk::COLORED;

ActivityDoItView::ActivityDoItView(IDisplay *display)
    : display_(display),
      activity_("??"),
      workout_("??") {
}

void ActivityDoItView::drawStatic() {
    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            paint.DrawHorizontalLine(13, 12, 270, COLORED);
            paint.DrawVerticalLine(148, 15, 110, COLORED);
            paint.DrawHorizontalLine(13, 70, 122, COLORED);
            paint.DrawHorizontalLine(161, 50, 122, COLORED);

            paint.DrawStringAt(14, 29, activity_.c_str(), &Font24, COLORED);
            paint.DrawStringAt(13, 91, workout_.c_str(), &Font24, COLORED);

            paint.DrawStringAt(189, 23, "GPS ??", &Font16, COLORED);
            paint.DrawStringAt(178, 61, "Press OK", &Font16, COLORED);
            paint.DrawStringAt(178, 99, "to start", &Font16, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
}

void ActivityDoItView::drawGNSSData(const GNSSData &data) {
    // GPS 3D
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "GPS %1d", data.fix_status);
            paint.DrawStringAt(189, 23, message, &Font16, COLORED);
        },
        {149, 13, 295, 49});
}

}  // namespace bk
