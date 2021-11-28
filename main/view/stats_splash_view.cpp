#include "stats_splash_view.h"

#include <paint.h>
namespace bk {
;

StatsSplashView::StatsSplashView(IDisplay *display) : display_(display) {
}

void StatsSplashView::drawStatic() {
    display_->enqueueDraw(
        [](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Statistics");
            paint.DrawStringAt(40, 104, message, &Font56, COLORED);
        });
}

}  // namespace bk
