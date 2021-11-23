#include "activity_splash_view.h"

#include <paint.h>
namespace bk {
;

ActivitySplashView::ActivitySplashView(IDisplay *display) : display_(display) {
}

void ActivitySplashView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Activities");
            paint.DrawStringAt(40, 104, message, &Font56, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});
}

}  // namespace bk
