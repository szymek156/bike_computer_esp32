#include "synchronize.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <paint.h>
#include <esp_log.h>

namespace bk {

;

SynchronizeView::SynchronizeView(IDisplay *display) : display_(display),
    ble_status_(VListWidget(display, Font42, {1, 169, 399, 239}, 1)) {
}

void SynchronizeView::drawStatic() {
    display_->enqueueDraw(
        [&](Paint &paint) {
            const int msg_size = 128;
            char message[msg_size];

            snprintf(message, msg_size, BT_SIGN);
            paint.DrawStringAt(184, 32, message, &Font56, COLORED);

            snprintf(message, msg_size, "status");
            paint.DrawStringAt(128, 111, message, &Font42, COLORED);

            // // Off
            // display_->enqueueDraw(
            //     [&](Paint &paint) {
            //         const int msg_size = 128;
            //         char message[msg_size];

            //     snprintf(message, msg_size, "Off");
            //     paint.DrawStringAt(164, 183, message, &Font42, COLORED);

            // },
            // {1, 169, 399, 239});

            snprintf(message, msg_size, "Off");
            paint.DrawStringAt(164, 183, message, &Font42, COLORED);
        },
        // Rectangle needs to cover whole widget area
        {0, 25, display_->getWidth(), display_->getHeight()});
}


VListWidget& SynchronizeView::getBleStatusList() {
    return ble_status_;
}

SynchronizePresenter::SynchronizePresenter(IDisplay *display, IEventDispatcher *events)
    : PagePresenter(events),
      view_(SynchronizeView(display)) {
}

SynchronizePresenter::~SynchronizePresenter() {
    ESP_LOGE("SynchronizePresenter", "DTOR");
}

void SynchronizePresenter::onEnter() {
    // TODO: get BT status, set correct element
    view_.getBleStatusList().updateElements({ "Off"});

    view_.drawStatic();
    events_->subForKeypad(this);
    events_->subForBLEEvents(this);
}

void SynchronizePresenter::onLeave() {
    events_->unSubForKeypad(this);
    events_->unSubForBLEEvents(this);
}

void SynchronizePresenter::onBluetoothChange(const BLEStatusData &data) {
    auto string_enum = std::array{"Off", "On", "Advertising", "Connected"};

    // Set one element
    view_.getBleStatusList().updateElements({string_enum[(int)data.status]});
    // By default active element has black background, clear the selection
    view_.getBleStatusList().clearSelection();
}

void SynchronizePresenter::onButtonPressed(const KeypadData &data) {
    // TODO: that's massive code duplication, it could be converted to strategy
    if (data.ru_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::next});
    } else if (data.lu_pressed) {
        events_->widgetEvent(WidgetData{.new_widget = WidgetData::prev});
    } else if (data.rd_pressed) {
        // events_->widgetEvent(WidgetData{.new_widget = WidgetData::more});
        // TODO: enable bluetooth
    } else if (data.ld_pressed) {
        // events_->widgetEvent(WidgetData{.new_widget = WidgetData::less});
        // TODO: disable bluetooth
    }
}
}  // namespace bk
