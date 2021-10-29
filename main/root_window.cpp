#include "root_window.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

RootWindow::RootWindow(IEventDispatcher *events, LayoutFactory *factory) : events_(events) {
    events_->subForWidgetChange(this);

    auto [status, curr] = factory->create();

    setStatusWidget(status);
    setCurrentWidget(curr);
}

RootWindow::~RootWindow() {
    events_->unSubForWidgetChange(this);
}

void RootWindow::onWidgetChange(const WidgetData &data) {
    if (current_widget_) {
        auto upcoming_widget = current_widget_->getWidget(data);

        if (upcoming_widget) {
            current_widget_->onLeave();

            current_widget_ = upcoming_widget;

            current_widget_->onEnter();
        } else {
            ESP_LOGE(TAG, "Got request to change a widget, but none was returned!");
        }
    }
}

void RootWindow::setStatusWidget(PresenterPtr status) {
    if (status_widget_) {
        status_widget_->onLeave();
    }

    status_widget_ = status;
    if (status_widget_) {
        status_widget_->onEnter();
    }
}

void RootWindow::setCurrentWidget(PresenterPtr current) {
    if (current_widget_) {
        current_widget_->onLeave();
    }

    current_widget_ = current;
    if (current_widget_) {
        current_widget_->onEnter();
    }
}

}  // namespace bk