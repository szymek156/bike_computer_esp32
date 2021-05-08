#pragma once
#include "event_dispatcher.h"
#include "layout_factory.h"
#include "presenter/page_presenter.h"

#include <memory>

#include <display.h>

namespace bk {
class RootWindow {
 public:
    RootWindow() {
    }

    ~RootWindow() = default;

    void setStatusWidget(PresenterPtr status) {
        if (status_widget_) {
            status_widget_->onLeave();
        }

        status_widget_ = status;
        if (status_widget_) {
            status_widget_->onEnter();
        }
    }

    void setCurrentWidget(PresenterPtr current) {
        if (current_widget_) {
            current_widget_->onLeave();
        }

        current_widget_ = current;
        if (current_widget_) {
            current_widget_->onEnter();
        }
    }

 private:
    // Status widget
    PresenterPtr status_widget_;

    // Main widget
    PresenterPtr current_widget_;
};
}  // namespace bk
