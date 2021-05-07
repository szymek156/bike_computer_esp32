#include "welcome_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

WelcomePresenter::WelcomePresenter(IDisplay *display, IEventDispatcher *events)
    : view_(WelcomeView(display)),
      events_(events) {
}

WelcomePresenter::~WelcomePresenter() {
}

void WelcomePresenter::onEnter() {
    view_.drawStatic();

    events_->subForWeather(this);
    events_->subForTime(this);
}

void WelcomePresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    events_->unSubForWeather(this);
    events_->unSubForTime(this);
}

void WelcomePresenter::onWeatherData(const WeatherData &data) {
    // TODO: Weather data is a sporadic event due to the swapping buffers in display, it may
    // happen that old data will be shown to the user - fix it, show always most recent data.
    view_.drawWeatherData(data);
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

    }
}
}  // namespace bk
