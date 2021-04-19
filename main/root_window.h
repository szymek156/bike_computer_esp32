#pragma once
#include "display.h"
#include "event_dispatcher.h"
#include "status_presenter.h"
namespace bk {
class RootWindow {
 public:
    RootWindow(IDisplay *display, IEventDispatcher *events)
        : display_(display),
          events_(events),
          status_presenter_(display_, events_) {
    }
    ~RootWindow() = default;

    void show() {
        status_presenter_.onEnter();
    }

 private:
    IDisplay *display_;
    IEventDispatcher *events_;

    StatusPresenter status_presenter_;
};
}  // namespace bk
