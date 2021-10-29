#include "activity_selected_presenter.h"

#include "activity_service.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

ActivitySelectedPresenter::ActivitySelectedPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(ActivitySelectedView(display)) {
}

ActivitySelectedPresenter::~ActivitySelectedPresenter() {
    ESP_LOGE("ActivitySelectedPresenter", "DTOR");
}

void ActivitySelectedPresenter::onEnter() {
    view_.getOptions().updateElements({"Do It", "View"});
    view_.setActivity(ActivityService::instance().getCurrentActivityType());
    view_.setWorkout(ActivityService::instance().getCurrentActivityWorkout());

    view_.drawStatic();

    events_->subForKeypad(this);
}

void ActivitySelectedPresenter::onLeave() {
    // TODO: this is a segfault, altering collection while iterating
    // Lost focus unsub for keypad events
    events_->unSubForKeypad(this);
}

void ActivitySelectedPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        view_.getOptions().goDown();
    } else if (data.lu_pressed) {
        view_.getOptions().goUp();
    } else if (data.rd_pressed) {
        // Go to next widget depending on the user selection
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more,
                                        .widget_idx = view_.getOptions().getCurrentSelectionIdx()});
    } else if (data.ld_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
