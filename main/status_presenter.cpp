#include "status_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

StatusPresenter::StatusPresenter(IDisplay *display, IEventDispatcher *events)
    : view_(StatusView(display)),
      events_(events) {
    events_->subForGNSS(this);
    events_->subForTime(this);
}

StatusPresenter::~StatusPresenter() {
    events_->unSubForGNSS(this);
    events_->unSubForTime(this);
}

void StatusPresenter::onEnter() {
    view_.drawStatic();
}

void StatusPresenter::onLeave() {
}

void StatusPresenter::onGNSSData(const GNSSData &data) {
    view_.drawGNSSData(data);
}
void StatusPresenter::onTimeData(const TimeData &data) {
    time_t now = {};
    struct tm timeinfo = {};

    time(&now);

    localtime_r(&now, &timeinfo);

    view_.drawDateTime(timeinfo);
    
}
}  // namespace bk
