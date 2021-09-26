#include "activity_workouts_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

ActivityWorkoutsPresenter::ActivityWorkoutsPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      // TODO: workouts are part of the model refactor later
      view_(ActivityWorkoutsView(display,
                                 {"5k", "10k", "Half Marathon", "Marathon", "Cooper Test"})) {
}

ActivityWorkoutsPresenter::~ActivityWorkoutsPresenter() {
    ESP_LOGE("ActivityWorkoutsPresenter", "DTOR");
}

void ActivityWorkoutsPresenter::onEnter() {
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
