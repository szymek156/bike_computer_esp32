#include "running_1_view.h"

#include <paint.h>
namespace bk {
;

Running1View::Running1View(IDisplay *display) : display_(display) {
}

void Running1View::drawStatic() {
    display_->enqueueDraw(
        [](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            paint.DrawHorizontalLine(13, 132, 174, COLORED);
            paint.DrawVerticalLine(200, 27, 210, COLORED);
            paint.DrawHorizontalLine(213, 132, 174, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "pace");
            paint.DrawStringAt(64, 35, message, &Font31, COLORED);

            snprintf(message, msg_size, "stride");
            paint.DrawStringAt(46, 143, message, &Font31, COLORED);

            snprintf(message, msg_size, "HR zone");
            paint.DrawStringAt(237, 35, message, &Font31, COLORED);

            snprintf(message, msg_size, "cadence");
            paint.DrawStringAt(237, 143, message, &Font31, COLORED);
        });
}

void Running1View::drawPace(float pace) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%4.2f", pace);
            paint.DrawStringAt(55, 89, message, &Font31, COLORED);
        });
}

void Running1View::drawStride(float stride) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%.2f", stride);
            paint.DrawStringAt(64, 197, message, &Font31, COLORED);
        });
}

void Running1View::drawHRZone(float zone) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%.2f", zone);
            paint.DrawStringAt(264, 89, message, &Font31, COLORED);
        });
}

void Running1View::drawCadence(int cadence) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%3d", cadence);
            paint.DrawStringAt(273, 197, message, &Font31, COLORED);
        });
}

}  // namespace bk
