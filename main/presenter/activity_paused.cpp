#include "activity_paused.h"

#include "activity_service.h"

#include <paint.h>
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

;

ActivityPausedView::ActivityPausedView(IDisplay *display)
    : display_(display),
      choices_(VListWidget(display, Font16, {149, 13, 295, 127}, 3, Alignment::Centered)) {
}

void ActivityPausedView::drawStatic() {
    display_->enqueueStaticDraw(
        [](Paint &paint) {
            paint.DrawHorizontalLine(13, 12, 270, COLORED);
            paint.DrawVerticalLine(148, 15, 110, COLORED);
            paint.DrawHorizontalLine(13, 70, 122, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 14, display_->getHeight(), display_->getWidth()});

    display_->enqueueStaticDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Paused");
            paint.DrawStringAt(23, 29, message, &Font24, COLORED);
        },
        {1, 13, 147, 69});

    // Will potentially overlay above statements
    choices_.drawStatic();
}

VListWidget &ActivityPausedView::getChoices() {
    return choices_;
}

ActivityPausedPresenter::ActivityPausedPresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(ActivityPausedView(display)) {
}

ActivityPausedPresenter::~ActivityPausedPresenter() {
    ESP_LOGE("ActivityPausedPresenter", "DTOR");
}

void ActivityPausedPresenter::onEnter() {
    view_.getChoices().updateElements({"Resume", "Save", "Discard"});

    view_.drawStatic();
    events_->subForKeypad(this);

    ActivityService::instance().pauseActivity();
}

void ActivityPausedPresenter::onLeave() {
    events_->unSubForKeypad(this);
}

void ActivityPausedPresenter::onButtonPressed(const KeypadData &data) {
    if (data.ru_pressed) {
        view_.getChoices().goDown();
    } else if (data.lu_pressed) {
        view_.getChoices().goUp();
    } else if (data.rd_pressed) {
        auto idx = view_.getChoices().getCurrentSelectionIdx();

        switch (idx) {
            case Resume: {
                ActivityService::instance().resumeActivity();
                events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
                break;
            }
            case Save: {
                ActivityService::instance().storeActivity();
                // TODO: show summary, then go to the welcome page
                events_->widgetEvent(
                    WidgetData{.new_widget = WidgetData::welcome, .widget_idx = idx});
                break;
            }
            case Discard: {
                ActivityService::instance().discardActivity();
                // TODO: show confirmation, then go to the welcome page
                events_->widgetEvent(
                    WidgetData{.new_widget = WidgetData::welcome, .widget_idx = idx});

                break;
            }
        }

    } else if (data.ld_pressed) {
        // events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
    }
}
}  // namespace bk
