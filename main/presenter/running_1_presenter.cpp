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

    // TODO: testing
    view_.drawCadence(69);
    view_.drawHRZone(4.2);
    view_.drawStride(1.23);
    view_.drawPace(4.27);

    events_->subForKeypad(this);
}

void Running1Presenter::onLeave() {
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
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
}  // namespace bk
