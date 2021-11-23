#include "welcome_view.h"

#include <paint.h>
namespace bk {
;

WelcomeView::WelcomeView(IDisplay *display) : display_(display) {
}

void WelcomeView::drawStatic() {
    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            // paint.DrawHorizontalLine(13, 132, 374, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});
}

void WelcomeView::drawDateTime(const tm &time_info) {
    // 21:37:07
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%T", &time_info);

            paint.DrawStringAt(8, 36, message, &Font83, COLORED);
        },
        {1, 25, 399, 131});

    // 02/09/21
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%d/%m/%y", &time_info);
            paint.DrawStringAt(8, 144, message, &Font83, COLORED);
        },
        {1, 133, 399, 239});

    // display_->enqueueDraw(
    //     [&](Paint &paint) {
    //         const int msg_size = 128;
    //         char message[msg_size];

    //         strftime(message, msg_size, "quick BROWN fox JUMPED over LAZY dog", &time_info);
    //         // paint.DrawStringAt(8, 30, message, &Font8, COLORED);

    //         // paint.DrawStringAt(8, 40, message, &Font12, COLORED);

    //         // paint.DrawStringAt(8, 55, message, &Font16, COLORED);

    //         paint.DrawStringAt(8, 30, message, &Font19, COLORED);

    //         // paint.DrawStringAt(8, 55, message, &Font24, COLORED);

    //         paint.DrawStringAt(8, 55, message, &Font31, COLORED);
    //         paint.DrawStringAt(8, 90, message, &Font42, COLORED);

    //         paint.DrawStringAt(8, 135, message, &Font56, COLORED);

    //         paint.DrawStringAt(8, 195, message, &Font83, COLORED);
    // },
    // {1, 133, 399, 239});
}
}  // namespace bk
