#include "stats_splash_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

StatsSplashPresenter::StatsSplashPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(StatsSplashView(display)) {
}

StatsSplashPresenter::~StatsSplashPresenter() {
    ESP_LOGE("StatsSplashPresenter", "DTOR");
}

void StatsSplashPresenter::onEnter() {
    view_.drawStatic();

    events_->subForKeypad(this);
}

void StatsSplashPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
}

void StatsSplashPresenter::onButtonPressed(const KeypadData &data) {
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
