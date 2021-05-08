#include "status_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

StatusPresenter::StatusPresenter(IDisplay *display, IEventDispatcher *events, RootWindow *root)
    : PagePresenter(events, root),
      view_(StatusView(display)) {
}

StatusPresenter::~StatusPresenter() {
}

void StatusPresenter::onEnter() {
    // TODO: needs to be drawn on both buffers, figure out how
    view_.drawStatic();

    events_->subForGNSS(this);
    events_->subForTime(this);
}

void StatusPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    events_->unSubForGNSS(this);
    events_->unSubForTime(this);
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
