#include "stats_presenter.h"
#include "activity_service.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

StatsPresenter::StatsPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(StatsView(display)) {
}

StatsPresenter::~StatsPresenter() {
    ESP_LOGE("StatsPresenter", "DTOR");
}

void StatsPresenter::onEnter() {
    auto activities = ActivityService::instance().getActivities();

    view_.getActivities().updateElements(activities);

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
