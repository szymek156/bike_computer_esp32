#include "stats_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

StatsPresenter::StatsPresenter(IDisplay *display, IEventDispatcher *events)
    : view_(StatsView(display)),
      events_(events) {
}

StatsPresenter::~StatsPresenter() {
}

void StatsPresenter::onEnter() {
    view_.drawStatic();

    events_->subForKeypad(this);
}

void StatsPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
}

void StatsPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {

    }
}
}  // namespace bk
