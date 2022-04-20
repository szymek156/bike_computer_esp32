#include "synchronize.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <paint.h>
#include <esp_log.h>

namespace bk {

;

SynchronizeView::SynchronizeView(IDisplay *display) : display_(display) {
}

void SynchronizeView::drawStatic() {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, BT_SIGN);
            paint.DrawStringAt(184, 32, message, &Font56, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});
}

SynchronizePresenter::SynchronizePresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(SynchronizeView(display)) {
}

SynchronizePresenter::~SynchronizePresenter() {
    ESP_LOGE("SynchronizePresenter", "DTOR");
}

void SynchronizePresenter::onEnter() {
    view_.drawStatic();
    events_->subForKeypad(this);
}

void SynchronizePresenter::onLeave() {
    events_->unSubForKeypad(this);
}

void SynchronizePresenter::onButtonPressed(const KeypadData &data) {
    // TODO: that's massive code duplication, it could be converted to strategy
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
