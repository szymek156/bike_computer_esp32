#include "test_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

TestPresenter::TestPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(TestView(display)) {
}

TestPresenter::~TestPresenter() {
    ESP_LOGE("TestPresenter", "DTOR");
}

void TestPresenter::onEnter() {
    view_.drawStatic();

    events_->subForWeather(this);
    events_->subForGNSS(this);
    events_->subForTime(this);
    events_->subForKeypad(this);
}

void TestPresenter::onLeave() {
    // TODO: be carefull to unsub to everything you need
    // particulary to keypad, forgetting that will generate
    // two keypad events from active and inactive displays
    // making behavior really spooky
    events_->unSubForWeather(this);
    events_->unSubForGNSS(this);
    events_->unSubForTime(this);
    events_->unSubForKeypad(this);
}

void TestPresenter::onGNSSData(const GNSSData &data) {
    view_.drawGNSSData(data);
}

void TestPresenter::onWeatherData(const WeatherData &data) {
    // TODO: Weather data is a sporadic event due to the swapping buffers in display, it may
    // happen that old data will be shown to the user - fix it, show always most recent data.
    view_.drawWeatherData(data);
}

void TestPresenter::onTimeData(const TimeData &data) {
    time_t now = {};
    struct tm timeinfo = {};

    time(&now);

    localtime_r(&now, &timeinfo);

    view_.drawDateTime(timeinfo);
}

void TestPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    } else if (data.lu_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::prev});
    }
}
}  // namespace bk
