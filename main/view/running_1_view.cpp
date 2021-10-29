#include "running_1_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

Running1View::Running1View(IDisplay *display) : display_(display) {
}

void Running1View::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(13, 70, 122, COLORED);
            paint.DrawVerticalLine(148, 15, 110, COLORED);
            paint.DrawHorizontalLine(161, 70, 122, COLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "pace");
            paint.DrawStringAt(52, 18, message, &Font16, COLORED);

            snprintf(message, msg_size, "stride");
            paint.DrawStringAt(41, 76, message, &Font16, COLORED);

            snprintf(message, msg_size, "HR zone");
            paint.DrawStringAt(183, 18, message, &Font16, COLORED);

            snprintf(message, msg_size, "cadence");
            paint.DrawStringAt(183, 76, message, &Font16, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
}

void Running1View::drawPace(float pace) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%4.2f", pace);
            paint.DrawStringAt(39, 45, message, &Font20, COLORED);
        },
        {1, 42, 147, 69});
}

void Running1View::drawStride(float stride) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%.2f", stride);
            paint.DrawStringAt(46, 103, message, &Font20, COLORED);
        },
        {1, 100, 147, 127});
}

void Running1View::drawHRZone(float zone) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%.2f", zone);
            paint.DrawStringAt(194, 45, message, &Font20, COLORED);
        },
        {149, 42, 295, 69});
}

void Running1View::drawCadence(int cadence) {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "%3d", cadence);
            paint.DrawStringAt(201, 103, message, &Font20, COLORED);
        },
        {149, 100, 295, 127});
}

}  // namespace bk
