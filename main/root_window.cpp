#include "root_window.h"

namespace bk {

RootWindow::RootWindow(IEventDispatcher *events) : events_(events) {
    events_->subForWidgetChange(this);
}

RootWindow::~RootWindow() {
    events_->unSubForWidgetChange(this);
}

void RootWindow::onWidgetChange(const WidgetData &data) {
    // TODO:
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