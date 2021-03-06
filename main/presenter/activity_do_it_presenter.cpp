#include "activity_do_it_presenter.h"

#include <activity_service.h>
#include <health_service.h>
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

ActivityDoItPresenter::ActivityDoItPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(ActivityDoItView(display)),
      layout_factory_(display, events_) {
}

ActivityDoItPresenter::~ActivityDoItPresenter() {
    ESP_LOGE("ActivityDoItPresenter", "DTOR");
}

void ActivityDoItPresenter::onEnter() {
    view_.setActivity(ActivityService::instance().getCurrentActivityType());
    view_.setWorkout(ActivityService::instance().getCurrentActivityWorkout());

    view_.drawStatic();

    // Create widgets according to the selected activity
    setMore(layout_factory_.create(ActivityService::instance().getCurrentActivity()));

    ActivityService::instance().newActivity();

    events_->subForGNSS(this);
    events_->subForKeypad(this);
}

void ActivityDoItPresenter::onLeave() {
    events_->unSubForKeypad(this);
    events_->unSubForGNSS(this);
}

void ActivityDoItPresenter::onGNSSData(const GNSSData &data) {
    view_.drawGNSSData(data);
}

void ActivityDoItPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    } else if (data.lu_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::prev});
    } else if (data.rd_pressed) {
        ActivityService::instance().startActivity();
        HealthService::reportAll();
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
    } else if (data.ld_pressed) {
        ActivityService::instance().discardActivity();

        events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
