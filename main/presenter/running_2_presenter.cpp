#include "running_2_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

Running2Presenter::Running2Presenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(Running2View(display)) {
}

Running2Presenter::~Running2Presenter() {
    ESP_LOGE("Running2Presenter", "DTOR");
}

void Running2Presenter::onEnter() {
    view_.drawStatic();

    events_->subForKeypad(this);
    events_->subForActivityData(this);
}

void Running2Presenter::onLeave() {
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
    events_->unSubForActivityData(this);
}

void Running2Presenter::onButtonPressed(const KeypadData &data) {
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

void Running2Presenter::onActivityData(const ActivityData &data) {
    view_.drawLapDistance(data.lap_distance);
    view_.drawLapTime(data.lap_time.hours, data.lap_time.minutes, data.lap_time.seconds);

    view_.drawTotalDistance(data.total_distance);
    view_.drawTotalTime(data.total_time.hours, data.total_time.minutes, data.total_time.seconds);
}

}  // namespace bk
