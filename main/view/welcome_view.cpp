#include "welcome_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

WelcomeView::WelcomeView(IDisplay *display) : display_(display) {
}

void WelcomeView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(13, 12, 270, COLORED);
            paint.DrawVerticalLine(148, 15, 52, COLORED);
            paint.DrawHorizontalLine(13, 70, 270, COLORED);
            paint.DrawVerticalLine(148, 73, 52, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            // Hit button below
            snprintf(message, msg_size, "Hit button below");
            paint.DrawStringAt(166, 73, message, &Font12, COLORED);

            // to calculate your
            snprintf(message, msg_size, "to calculate your");
            paint.DrawStringAt(162, 92, message, &Font12, COLORED);

            // BMI
            snprintf(message, msg_size, "BMI");
            paint.DrawStringAt(205, 109, message, &Font16, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
}

void WelcomeView::drawWeatherData(const WeatherData &data) {
    // 23.19[*C]
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%5.2f[*C]", data.temp_c);
            paint.DrawStringAt(11, 74, message, &Font20, COLORED);
        },
        {1, 71, 147, 98});

    // 133.94[m]
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%5.2f[m]", data.altitude_m);
            paint.DrawStringAt(11, 103, message, &Font20, COLORED);
        },
        {1, 100, 147, 127});
}

void WelcomeView::drawDateTime(const tm &time_info) {
    // 02/09/21
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%d/%m/%y", &(time_info));
            paint.DrawStringAt(6, 14, message, &Font24, COLORED);
        },
        {1, 13, 147, 40});

    // 19:34:20
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%T", &(time_info));
            paint.DrawStringAt(6, 43, message, &Font24, COLORED);
        },
        {1, 42, 147, 69});
}

void WelcomeView::drawGNSSData(const GNSSData &data) {
    // in view / tracked
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "in view / tracked");
            paint.DrawStringAt(162, 20, message, &Font12, COLORED);
        },
        {149, 13, 295, 40});

    // 13 / 11
    display_->enqueueDraw(
        [&data](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%d / %d", data.sats_in_view, data.sats_tracked);
            paint.DrawStringAt(162, 43, message, &Font24, COLORED);
        },
        {149, 42, 295, 69});
}
}  // namespace bk
