#include "workout_steps_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <paint.h>
#include <esp_log.h>

namespace bk {

;

WorkoutStepsView::WorkoutStepsView(IDisplay *display)
    : display_(display),
      workout_steps_(VListWidget(display, Font24, {1, 25, display_->getWidth(), display_->getHeight()}, 4, Alignment::Left)) {
}

void WorkoutStepsView::drawStatic() {
    workout_steps_.drawStatic();
}

VListWidget &WorkoutStepsView::getWorkoutSteps() {
    return workout_steps_;
}

WorkoutStepsPresenter::WorkoutStepsPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(WorkoutStepsView(display)) {
}

WorkoutStepsPresenter::~WorkoutStepsPresenter() {
    ESP_LOGE("WorkoutStepsPresenter", "DTOR");
}

void WorkoutStepsPresenter::onEnter() {
    // TODO: get that from the activity service
    view_.getWorkoutSteps().updateElements({"run 5.00 1/5",
                                            "cool down 2 minutes",
                                            "run 5.00 2/5",
                                            "cool down 2 minutes",
                                            "run 5.00 3/5",
                                            "cool down 2 minutes",
                                            "run 5.00 4/5",
                                            "cool down 2 minutes",
                                            "run 5.00 5/5",
                                            "cool down 2 minutes"});

    view_.drawStatic();
    events_->subForKeypad(this);
}

void WorkoutStepsPresenter::onLeave() {
    events_->unSubForKeypad(this);
}

void WorkoutStepsPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        view_.getWorkoutSteps().goDown();
    } else if (data.lu_pressed) {
        view_.getWorkoutSteps().goUp();
    } else if (data.rd_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
