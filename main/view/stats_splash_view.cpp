#include "stats_splash_view.h"

#include <paint.h>
namespace bk {
using bk::COLORED;

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
        {0, 14, display_->getHeight(), display_->getWidth()});
}

}  // namespace bk
