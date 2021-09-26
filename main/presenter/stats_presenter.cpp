#include "stats_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

StatsPresenter::StatsPresenter(IDisplay *display,
                               IEventDispatcher *events,
                               const std::vector<std::string> &activities)
    : PagePresenter(events),
      view_(StatsView(display, activities)) {
}

StatsPresenter::~StatsPresenter() {
    ESP_LOGE("StatsPresenter", "DTOR");
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
        view_.getActivities().goDown();
    } else if (data.lu_pressed) {
        view_.getActivities().goUp();
    } else if (data.rd_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
