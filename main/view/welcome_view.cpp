#include "welcome_view.h"

#include <paint.h>
namespace bk {
;

WelcomeView::WelcomeView(IDisplay *display) : display_(display) {
}

void WelcomeView::drawStatic() {
    // display_->enqueueStaticDraw(
    // [](Paint &paint) {
    // paint.DrawHorizontalLine(13, 12, 270, COLORED);
    // paint.DrawHorizontalLine(13, 70, 270, COLORED);
    // },
    // Rectangle needs to cover whole widget area
    // {0, 14, display_->getHeight(), display_->getWidth()});
}

void WelcomeView::drawDateTime(const tm &time_info) {
    // 21:37:07
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            strftime(message, msg_size, "%T", &time_info);
            paint.DrawStringAt(1, 15, message, &Font48, COLORED);

            strftime(message, msg_size, "%T", &time_info);
            paint.DrawStringAt(1, 80, message, &Font72, COLORED);
        },
        {1, 13, display_->getWidth(), display_->getHeight()});


    // display_->enqueueDraw(
    //     [&](Paint &paint) {
    //         const int msg_size = 128;
    //         char message[msg_size];

    //         strftime(message, msg_size, "%T", &time_info);
    //         paint.DrawStringAt(80, 15, message, &Font72, COLORED);
    //     },
    //     {1, 13, display_->getHeight(), display_->getWidth()});

    // // 02/09/21
    // display_->enqueueDraw(
    //     [&](Paint &paint) {
    //         const int msg_size = 128;
    //         char message[msg_size];

    //         strftime(message, msg_size, "%d/%m/%y", &time_info);
    //         paint.DrawStringAt(80, 87, message, &Font24, COLORED);
    //     },
    // {1, 71, 295, 127});
}
}  // namespace bk
