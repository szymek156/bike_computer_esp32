#include "stats_selected_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

StatsSelectedPresenter::StatsSelectedPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(StatsSelectedView(display)) {
}

StatsSelectedPresenter::~StatsSelectedPresenter() {
    ESP_LOGE("StatsSelectedPresenter", "DTOR");
}

void StatsSelectedPresenter::onEnter() {
    view_.drawStatic();

    events_->subForKeypad(this);
}

void StatsSelectedPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
}

void StatsSelectedPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        // view_.getTextArea().goDown();
    } else if (data.lu_pressed) {
        // view_.getTextArea().goUp();
    } else if (data.rd_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
