#pragma once
#include "event_dispatcher.h"
#include "layout_factory.h"

#include <memory>

#include <display.h>

namespace bk {
class RootWindow {
 public:
    RootWindow(LayoutFactory &factory)  {
        auto [status, curr] = factory.create();

        status_presenter_ = std::move(status);
        current_widget_ = std::move(curr);
    }

    ~RootWindow() = default;

    void show() {
        status_presenter_->onEnter();
        current_widget_->onEnter();
    }

 private:
    // Status widget
    std::unique_ptr<IPagePresenter> status_presenter_;

    // Main widget
    std::unique_ptr<IPagePresenter> current_widget_;
};
}  // namespace bk
