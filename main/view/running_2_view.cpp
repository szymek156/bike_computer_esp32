#include "running_2_view.h"

#include <paint.h>
namespace bk {
;

Running2View::Running2View(IDisplay *display) : display_(display) {
}

void Running2View::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(13, 70, 122, COLORED);
            paint.DrawVerticalLine(148, 15, 110, COLORED);
            paint.DrawHorizontalLine(161, 70, 122, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "total dist");
            paint.DrawStringAt(19, 18, message, &Font16, COLORED);

            snprintf(message, msg_size, "lap dist");
            paint.DrawStringAt(30, 76, message, &Font16, COLORED);
            snprintf(message, msg_size, "total time");
            paint.DrawStringAt(167, 18, message, &Font16, COLORED);

            snprintf(message, msg_size, "lap time");
            paint.DrawStringAt(178, 76, message, &Font16, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
}

void Running2View::drawTotalDistance(float total_distance) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%.2f", total_distance);
            paint.DrawStringAt(39, 45, message, &Font20, COLORED);
        },
        {1, 42, 147, 69});
}

void Running2View::drawLapDistance(float lap_distance) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%.2f", lap_distance);
            paint.DrawStringAt(46, 103, message, &Font20, COLORED);
        },
        {1, 100, 147, 127});
}

void Running2View::drawTotalTime(int hours, int minutes, int seconds) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%02d:%02d:%02d", hours, minutes, seconds);
            paint.DrawStringAt(166, 45, message, &Font20, COLORED);
        },
        {149, 42, 295, 69});
}

void Running2View::drawLapTime(int hours, int minutes, int seconds) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%02d:%02d:%02d", hours, minutes, seconds);
            paint.DrawStringAt(166, 103, message, &Font20, COLORED);
        },
        {149, 100, 295, 127});
}

}  // namespace bk
