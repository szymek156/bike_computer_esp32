#include "select_activity_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

SelectActivityPresenter::SelectActivityPresenter(IDisplay *display,
                                                 IEventDispatcher *events,
                                                 const std::vector<std::string> &activities)
    : PagePresenter(events),
      view_(SelectActivityView(display)),
      activities_presenter_(VListPresenter(activities)) {
}

SelectActivityPresenter::~SelectActivityPresenter() {
    ESP_LOGE("SelectActivityPresenter", "DTOR");
}

void SelectActivityPresenter::onEnter() {
    view_.drawStatic();
    events_->subForKeypad(this);
}

void SelectActivityPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    events_->unSubForKeypad(this);
}

void SelectActivityPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    }
}
}  // namespace bk
