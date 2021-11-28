#include "TEMPLATE_view.h"

#include <paint.h>
namespace bk {
;

TEMPLATEView::TEMPLATEView(IDisplay *display) : display_(display) {
}

void TEMPLATEView::drawStatic() {
    display_->enqueueDraw(
        [](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "TEMPLATE");
            paint.DrawStringAt(50, paint.GetWidth() / 2 - 6, message, &Font24, COLORED);
        });
}

}  // namespace bk
