#include "root_window.h"

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
        current_widget_->onLeave();
        current_widget_ = current_widget_->getWidget(data.new_widget);
    }

    if (current_widget_) {
        current_widget_->onEnter();
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