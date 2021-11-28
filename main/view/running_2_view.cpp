#include "running_2_view.h"

#include <paint.h>
namespace bk {
;

Running2View::Running2View(IDisplay *display) : display_(display) {
}

void Running2View::drawStatic() {
    display_->enqueueDraw(
        [](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            paint.DrawHorizontalLine(13, 132, 174, COLORED);
            paint.DrawVerticalLine(200, 27, 210, COLORED);
            paint.DrawHorizontalLine(213, 132, 174, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "total dist");
            paint.DrawStringAt(10, 35, message, &Font31, COLORED);
            snprintf(message, msg_size, "lap dist");
            paint.DrawStringAt(28, 143, message, &Font31, COLORED);
            snprintf(message, msg_size, "total time");
            paint.DrawStringAt(210, 35, message, &Font31, COLORED);
            snprintf(message, msg_size, "lap time");
            paint.DrawStringAt(228, 143, message, &Font31, COLORED);
        });
}

void Running2View::drawTotalDistance(float total_distance) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%.2f", total_distance);
            paint.DrawStringAt(55, 89, message, &Font31, COLORED);
        });
}

void Running2View::drawLapDistance(float lap_distance) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%.2f", lap_distance);
            paint.DrawStringAt(64, 197, message, &Font31, COLORED);
        });
}

void Running2View::drawTotalTime(int hours, int minutes, int seconds) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%02d:%02d:%02d", hours, minutes, seconds);
            paint.DrawStringAt(228, 89, message, &Font31, COLORED);
        });
}

void Running2View::drawLapTime(int hours, int minutes, int seconds) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%02d:%02d:%02d", hours, minutes, seconds);
            paint.DrawStringAt(228, 197, message, &Font31, COLORED);
        });
}

}  // namespace bk
