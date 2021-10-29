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

    view_.drawLapTime(2, 14, 34);
    view_.drawLapDistance(15.06);
    view_.drawTotalDistance(21.37);
    view_.drawTotalTime(2, 3, 4);
    events_->subForKeypad(this);
}

void Running2Presenter::onLeave() {
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
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
}  // namespace bk
