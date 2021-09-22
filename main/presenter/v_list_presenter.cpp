#include "select_activity_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

VListPresenter::VListPresenter(IDisplay *display,
                               IEventDispatcher *events,
                               const std::vector<std::string> &elements)
    : PagePresenter(events),
      view_(VListView(display, elements)),
      elements_(elements) {
}

VListPresenter::~VListPresenter() {
    ESP_LOGE("VListPresenter", "DTOR");
}

void VListPresenter::onEnter() {
    view_.drawStatic();
    events_->subForKeypad(this);
}

void VListPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    events_->unSubForKeypad(this);
}

void VListPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    }
}
}  // namespace bk
