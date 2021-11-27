#include "workout_steps_splash.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>
#include <paint.h>

namespace bk {

;

WorkoutStepsSplashView::WorkoutStepsSplashView(IDisplay *display) : display_(display) {
}

void WorkoutStepsSplashView::drawStatic() {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Workout Steps");
            paint.DrawStringAt(44, 111, message, &Font42, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});
}

WorkoutStepsSplashPresenter::WorkoutStepsSplashPresenter(IDisplay *display,
                                                         IEventDispatcher *events)
    : PagePresenter(events),
      view_(WorkoutStepsSplashView(display)) {
}

WorkoutStepsSplashPresenter::~WorkoutStepsSplashPresenter() {
    ESP_LOGE("WorkoutStepsSplashPresenter", "DTOR");
}

void WorkoutStepsSplashPresenter::onEnter() {
    view_.drawStatic();
    events_->subForKeypad(this);
}

void WorkoutStepsSplashPresenter::onLeave() {
    events_->unSubForKeypad(this);
}

void WorkoutStepsSplashPresenter::onButtonPressed(const KeypadData &data) {
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
