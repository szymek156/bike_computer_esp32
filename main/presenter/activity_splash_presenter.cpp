#include "activity_splash_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

ActivitySplashPresenter::ActivitySplashPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(ActivitySplashView(display)) {
}

ActivitySplashPresenter::~ActivitySplashPresenter() {
    ESP_LOGE("ActivitySplashPresenter", "DTOR");
}

void ActivitySplashPresenter::onEnter() {
    view_.drawStatic();

    events_->subForKeypad(this);
}

void ActivitySplashPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
}

void ActivitySplashPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    } else if (data.lu_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::prev});
    } else if (data.rd_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.rd_pressed) {
    }
}
}  // namespace bk
