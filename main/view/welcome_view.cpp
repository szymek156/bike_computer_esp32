#include "welcome_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

WelcomeView::WelcomeView(IDisplay *display) : display_(display) {
}

void WelcomeView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            // TODO: Those arcane computations shall be done by tool
            const int MARGIN = 8;
            paint.DrawHorizontalLine(
                MARGIN, paint.GetWidth() / 2 + 6, paint.GetHeight() - 2 * MARGIN, COLORED);
            paint.DrawVerticalLine(
                paint.GetHeight() / 2, 13 + MARGIN, paint.GetWidth() - 13 - 2 * MARGIN, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getWidth(), display_->getHeight()});
}

void WelcomeView::drawWeatherData(const WeatherData &data) {
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "SENSOR:");
            paint.DrawStringAt(0, 78, message, &Font20, COLORED);

            snprintf(message, msg_size, "TMP   %5.2f", data.temp_c);
            paint.DrawStringAt(0, 95, message, &Font16, COLORED);

            snprintf(message, msg_size, "ALT  %7.2f", data.altitude_m);
            paint.DrawStringAt(0, 110, message, &Font16, COLORED);
        },
        {0, 78, 130, 256});
}

void WelcomeView::drawDateTime(const tm &time_info) {
    display_->enqueueDraw(
        [&time_info](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];
        },
        {0, 0, 0, 0});
}
}  // namespace bk
