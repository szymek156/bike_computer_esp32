#include "activity_splash_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

ActivitySplashView::ActivitySplashView(IDisplay *display) : display_(display) {
}

void ActivitySplashView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Activities");
            paint.DrawStringAt(63, 58, message, &Font24, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
}

}  // namespace bk