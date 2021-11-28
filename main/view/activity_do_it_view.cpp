#include "activity_do_it_view.h"

#include "freertos/FreeRTOS.h"

#include <paint.h>
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>
namespace bk {
;

ActivityDoItView::ActivityDoItView(IDisplay *display)
    : display_(display),
      activity_("??"),
      workout_("??") {
}

void ActivityDoItView::drawStatic() {
    display_->enqueueDraw(
        [&](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            paint.DrawVerticalLine(200, 27, 210, COLORED);
            paint.DrawHorizontalLine(13, 132, 174, COLORED);
            paint.DrawHorizontalLine(213, 96, 174, COLORED);

            paint.DrawStringAt(16, 57, activity_.c_str(), &Font42, COLORED);
            paint.DrawStringAt(1, 170, workout_.c_str(), &Font31, COLORED);

            paint.DrawStringAt(258, 48, "GPS ??", &Font24, COLORED);
            paint.DrawStringAt(244, 120, "Press OK", &Font24, COLORED);
            paint.DrawStringAt(244, 192, "to start", &Font24, COLORED);
        });
}

void ActivityDoItView::drawGNSSData(const GNSSData &data) {
    // GPS 3D
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "GPS %1d", data.fix_status);
            paint.DrawStringAt(258, 48, message, &Font24, COLORED);
        });
}

}  // namespace bk
