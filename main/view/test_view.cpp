#include "test_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

TestView::TestView(IDisplay *display) : display_(display) {
}

void TestView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(13, 12, 270, COLORED);
            paint.DrawVerticalLine(148, 15, 52, COLORED);
            paint.DrawHorizontalLine(13, 70, 270, COLORED);
            paint.DrawVerticalLine(148, 73, 52, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
}

void TestView::drawWeatherData(const WeatherData &data) {
    // 23.19[*C]
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%5.2f[*C]", data.temp_c);
            paint.DrawStringAt(11, 74, message, &Font20, COLORED);
        },
        {1, 71, 147, 98});

    // 133.94[m]
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%07.2f[m]", data.altitude_m);
            paint.DrawStringAt(11, 103, message, &Font20, COLORED);
        },
        {1, 100, 147, 127});
}

void TestView::drawDateTime(const tm &time_info) {
    // 02/09/21
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%d/%m/%y", &(time_info));

            paint.DrawStringAt(18, 16, message, &Font20, COLORED);
        },
        {1, 13, 147, 40});

    // 19:34:20
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%T", &(time_info));
            paint.DrawStringAt(18, 45, message, &Font20, COLORED);
        },
        {1, 42, 147, 69});
}

void TestView::drawGNSSData(const GNSSData &data) {
    // in view: 13
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "in view: %d", data.sats_in_view);
            paint.DrawStringAt(148, 16, message, &Font20, COLORED);
        },
        {149, 13, 295, 40});

    // tracked: 11
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "tracked: %d", data.sats_tracked);
            paint.DrawStringAt(148, 45, message, &Font20, COLORED);
        },
        {149, 42, 295, 69});
}
}  // namespace bk
