#include "test_view.h"

#include <paint.h>
namespace bk {

TestView::TestView(IDisplay *display) : display_(display) {
}

void TestView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawVerticalLine(200, 27, 102, COLORED);
            paint.DrawHorizontalLine(13, 132, 374, COLORED);
            paint.DrawVerticalLine(200, 135, 102, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});
}

void TestView::drawWeatherData(const WeatherData &data) {
    // 23.19[*C]
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%5.2f[*C]", data.temp_c);
            paint.DrawStringAt(37, 147, message, &Font24, COLORED);
        },
        {1, 133, 199, 185});

    // 133.94[m]
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%07.2f[m]", data.altitude_m);
            paint.DrawStringAt(30, 201, message, &Font24, COLORED);
        },
        {1, 187, 199, 239});
}

void TestView::drawDateTime(const tm &time_info) {
    // 02/09/21
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%d/%m/%y", &(time_info));

            paint.DrawStringAt(44, 39, message, &Font24, COLORED);
        },
        {1, 25, 199, 77});

    // 19:34:20
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%T", &(time_info));
            paint.DrawStringAt(44, 93, message, &Font24, COLORED);
        },
        {1, 79, 199, 131});
}

void TestView::drawGNSSData(const GNSSData &data) {
    // in view: 13
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "in view: %d", data.sats_in_view);
            paint.DrawStringAt(223, 39, message, &Font24, COLORED);
        },
        {201, 25, 399, 77});

    // tracked: 11
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "tracked: %d", data.sats_tracked);
            paint.DrawStringAt(223, 93, message, &Font24, COLORED);
        },
        {201, 79, 399, 131});
}
}  // namespace bk
