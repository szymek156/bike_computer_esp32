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

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Hit button below");
            paint.DrawStringAt(160, 77, message, &Font12, COLORED);

            snprintf(message, msg_size, "to calculate your");
            paint.DrawStringAt(160, 92, message, &Font12, COLORED);

            snprintf(message, msg_size, "BMI");
            paint.DrawStringAt(200, 106, message, &Font16, COLORED);
            
            
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
}

void WelcomeView::drawWeatherData(const WeatherData &data) {
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%5.2f[*C]", data.temp_c);
            paint.DrawStringAt(18, 77, message, &Font20, COLORED);

            snprintf(message, msg_size, "%5.2f[m]", data.altitude_m);
            paint.DrawStringAt(18, 105, message, &Font20, COLORED);
        },
        {0, 78, 130, 256});
}

void WelcomeView::drawDateTime(const tm &time_info) {
    display_->enqueueDraw(
        [&time_info](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%d/%m/%y", &(time_info));
            paint.DrawStringAt(18, 20, message, &Font20, COLORED);

            strftime(message, msg_size, "%T", &(time_info));
            paint.DrawStringAt(18, 47, message, &Font20, COLORED);
        },
        {18, 20, 130, 67});
}

void WelcomeView::drawGNSSData(const GNSSData &data) {
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "in view / tracked");
            paint.DrawStringAt(160, 20, message, &Font12, COLORED);

            snprintf(message, msg_size, "%d / %d", data.sats_in_view, data.sats_tracked);
            paint.DrawStringAt(200, 40, message, &Font12, COLORED);
        },
        {160, 20, 256, 67});
}
}  // namespace bk
