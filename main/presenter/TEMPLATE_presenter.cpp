#include "TEMPLATE_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

TEMPLATEPresenter::TEMPLATEPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(TEMPLATEView(display)) {
}

TEMPLATEPresenter::~TEMPLATEPresenter() {
    ESP_LOGE("TEMPLATEPresenter", "DTOR" );
}

void TEMPLATEPresenter::onEnter() {
    view_.drawStatic();

    events_->subForKeypad(this);
}

void TEMPLATEPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
}

void TEMPLATEPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    } else if (data.lu_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::prev});
    }
}
}  // namespace bk
