#include "TEMPLATE_view.h"

#include <epdpaint.h>
namespace bk {
using bk::COLORED;

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
        {0, 14, display_->getHeight(), display_->getWidth()});
}

}  // namespace bk
