#include "running_1_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

Running1Presenter::Running1Presenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(Running1View(display)) {
}

Running1Presenter::~Running1Presenter() {
    ESP_LOGE("Running1Presenter", "DTOR");
}

void Running1Presenter::onEnter() {
    view_.drawStatic();

    view_.drawCadence(0);
    view_.drawHRZone(0);
    view_.drawStride(0);
    view_.drawPace(0);

    events_->subForKeypad(this);
    events_->subForActivityData(this);
}

void Running1Presenter::onLeave() {
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
    events_->unSubForActivityData(this);
}

void Running1Presenter::onButtonPressed(const KeypadData &data) {
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

void Running1Presenter::onActivityData(const ActivityData &data) {
    // To convert m/s to minutes/km (pace)
    // m/s -> km/h X * 3.6
    // km/h -> m / km 60 / (X * 3,6)
    // 60 / 3.6 = 16.(6)
    // m/s -> m / km
    // 16.(6) / X
    view_.drawPace(16.66666666666 / data.speed_ms);
}

}  // namespace bk
