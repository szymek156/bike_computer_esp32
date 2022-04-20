#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"

#include <display.h>

namespace bk {
class SynchronizeView {
 public:
    SynchronizeView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
};

class SynchronizePresenter : public PagePresenter, public KeypadListener {
 public:
    SynchronizePresenter(IDisplay *display, IEventDispatcher *events);
    ~SynchronizePresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    SynchronizeView view_;
};

}  // namespace bk
