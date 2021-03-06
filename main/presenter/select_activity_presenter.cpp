#include "select_activity_presenter.h"

#include "activity_service.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

SelectActivityPresenter::SelectActivityPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(SelectActivityView(display)) {
}

SelectActivityPresenter::~SelectActivityPresenter() {
    ESP_LOGE("SelectActivityPresenter", "DTOR");
}

void SelectActivityPresenter::onEnter() {
    auto activities = ActivityService::instance().getActivities();
    view_.getActivities().updateElements(activities);
    view_.drawStatic();
    events_->subForKeypad(this);
}

void SelectActivityPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    events_->unSubForKeypad(this);
}

void SelectActivityPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        view_.getActivities().goDown();
    } else if (data.lu_pressed) {
        view_.getActivities().goUp();
    } else if (data.rd_pressed) {
        // Remember selected activity
        ActivityService::instance().setCurrentActivityType(
            view_.getActivities().getCurrentSelection());
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
