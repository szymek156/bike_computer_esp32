#include "workout_steps_splash.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <paint.h>
#include <esp_log.h>

namespace bk {

using bk::COLORED;

WorkoutStepsSplashView::WorkoutStepsSplashView(IDisplay *display) : display_(display) {
}

void WorkoutStepsSplashView::drawStatic() {
    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Workout Steps");
            paint.DrawStringAt(37, 58, message, &Font24, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});
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
