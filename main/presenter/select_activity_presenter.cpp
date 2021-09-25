#include "select_activity_presenter.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

SelectActivityPresenter::SelectActivityPresenter(IDisplay *display,
                                                 IEventDispatcher *events,
                                                 const std::vector<std::string> &activities)
    : PagePresenter(events),
      view_(SelectActivityView(display, activities)) {
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
        view_.getActivities().goDown();
    } else if (data.lu_pressed) {
        view_.getActivities().goUp();
    } else if (data.rd_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
