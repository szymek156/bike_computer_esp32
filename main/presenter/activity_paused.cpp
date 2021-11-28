#include "activity_paused.h"

#include "activity_service.h"

#include <paint.h>
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

;

ActivityPausedView::ActivityPausedView(IDisplay *display)
    : display_(display),
      choices_(VListWidget(display, Font24, {201, 25, 399, 239}, 3, Alignment::Centered)) {
}

void ActivityPausedView::drawStatic() {
    display_->enqueueDraw(
        [](Paint &paint) {
            // Rectangle needs to cover whole widget area
            paint.DrawFilledRectangle(0, 25, paint.GetWidth(), paint.GetHeight(), UNCOLORED);

            paint.DrawHorizontalLine(13, 24, 374, COLORED);
            paint.DrawVerticalLine(200, 27, 210, COLORED);
            paint.DrawHorizontalLine(13, 132, 174, COLORED);
        });

    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, "Paused");
            paint.DrawStringAt(28, 57, message, &Font42, COLORED);
        });

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
