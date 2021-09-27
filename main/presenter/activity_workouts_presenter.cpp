#include "activity_workouts_presenter.h"

#include "activity_service.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

ActivityWorkoutsPresenter::ActivityWorkoutsPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      // TODO: workouts are part of the model refactor later
      view_(ActivityWorkoutsView(display)) {
}

ActivityWorkoutsPresenter::~ActivityWorkoutsPresenter() {
    ESP_LOGE("ActivityWorkoutsPresenter", "DTOR");
}

void ActivityWorkoutsPresenter::onEnter() {
    view_.getWorkouts().updateElements(ActivityService::instance().getWorkouts());
    view_.setActivity(ActivityService::instance().getCurrentActivityType());
    view_.drawStatic();

    events_->subForKeypad(this);
}

void ActivityWorkoutsPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
}

void ActivityWorkoutsPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        view_.getWorkouts().goDown();
    } else if (data.lu_pressed) {
        view_.getWorkouts().goUp();
    } else if (data.rd_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
