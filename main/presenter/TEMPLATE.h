#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"

#include <display.h>

namespace bk {
class TEMPLATEView {
 public:
    TEMPLATEView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
};

class TEMPLATEPresenter : public PagePresenter, public KeypadListener {
 public:
    TEMPLATEPresenter(IDisplay *display, IEventDispatcher *events);
    ~TEMPLATEPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    TEMPLATEView view_;
};

}  // namespace bk
