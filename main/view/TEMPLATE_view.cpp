#include "TEMPLATE_view.h"

#include <paint.h>
namespace bk {
;

TEMPLATEView::TEMPLATEView(IDisplay *display) : display_(display) {
}

void TEMPLATEView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "TEMPLATE");
            paint.DrawStringAt(50, paint.GetWidth() / 2 - 6, message, &Font24, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});
}

}  // namespace bk
