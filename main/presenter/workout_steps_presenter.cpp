#include "workout_steps_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>
#include <epdpaint.h>

namespace bk {

using bk::COLORED;

WorkoutStepsView::WorkoutStepsView(IDisplay *display) : display_(display) {
}

void WorkoutStepsView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "WorkoutSteps");
            paint.DrawStringAt(50, paint.GetWidth() / 2 - 6, message, &Font24, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
}

WorkoutStepsPresenter::WorkoutStepsPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(WorkoutStepsView(display)) {
}

WorkoutStepsPresenter::~WorkoutStepsPresenter() {
    ESP_LOGE("WorkoutStepsPresenter", "DTOR");
}

void WorkoutStepsPresenter::onEnter() {
    view_.drawStatic();

    events_->subForKeypad(this);
}

void WorkoutStepsPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
}

void WorkoutStepsPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    } else if (data.lu_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::prev});
    } else if (data.rd_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
