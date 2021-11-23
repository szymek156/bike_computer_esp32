#include "stats_splash_view.h"

#include <paint.h>
namespace bk {
;

StatsSplashView::StatsSplashView(IDisplay *display) : display_(display) {
}

void StatsSplashView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Statistics");
            paint.DrawStringAt(63, 58, message, &Font24, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});
}

}  // namespace bk
