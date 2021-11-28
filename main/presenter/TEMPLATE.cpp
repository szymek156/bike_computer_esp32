#include "TEMPLATE.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <paint.h>
#include <esp_log.h>

namespace bk {

;

TEMPLATEView::TEMPLATEView(IDisplay *display) : display_(display) {
}

void TEMPLATEView::drawStatic() {
    display_->enqueueDraw(
        [&](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "TEMPLATE");
            paint.DrawStringAt(80, 58, message, &Font24, COLORED);
        });
}

TEMPLATEPresenter::TEMPLATEPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(TEMPLATEView(display)) {
}

TEMPLATEPresenter::~TEMPLATEPresenter() {
    ESP_LOGE("TEMPLATEPresenter", "DTOR");
}

void TEMPLATEPresenter::onEnter() {
    view_.drawStatic();
    events_->subForKeypad(this);
}

void TEMPLATEPresenter::onLeave() {
    events_->unSubForKeypad(this);
}

void TEMPLATEPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    } else if (data.lu_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::prev});
    } else if (data.rd_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
