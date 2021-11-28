#include "welcome_view.h"

#include <paint.h>
namespace bk {
;

WelcomeView::WelcomeView(IDisplay *display) : display_(display) {
}

void WelcomeView::drawStatic() {
    display_->enqueueDraw([&](Paint &paint) {
        // Rectangle needs to cover whole widget area
        paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

        paint.DrawHorizontalLine(13, 132, 374, COLORED);
    });
}

void WelcomeView::drawDateTime(const tm &time_info) {
    // 21:37:07
    display_->enqueueDraw([&](Paint &paint) {
        const int msg_size = 128;
        char message[msg_size];

        strftime(message, msg_size, "%T", &time_info);

        paint.DrawStringAt(72, 50, message, &Font56, COLORED);
    });

    // 02/09/21
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%d/%m/%y", &time_info);
            paint.DrawStringAt(72, 158, message, &Font56, COLORED);
        });
}
}  // namespace bk
