#include "welcome_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

WelcomePresenter::WelcomePresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(WelcomeView(display)) {
}

WelcomePresenter::~WelcomePresenter() {
    ESP_LOGE("WelcomePresenter", "DTOR" );
}

void WelcomePresenter::onEnter() {
    view_.drawStatic();

    events_->subForTime(this);
    events_->subForKeypad(this);
}

void WelcomePresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    events_->unSubForTime(this);
    events_->unSubForKeypad(this);
}

void WelcomePresenter::onTimeData(const TimeData &data) {
    time_t now = {};
    struct tm timeinfo = {};

    time(&now);

    localtime_r(&now, &timeinfo);

    view_.drawDateTime(timeinfo);
}

void WelcomePresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    } else if (data.lu_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::prev});
    }
}
}  // namespace bk
