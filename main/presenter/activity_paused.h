#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "widgets/v_list_widget.h"

#include <display.h>

namespace bk {
class ActivityPausedView {
 public:
    ActivityPausedView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

    VListWidget& getChoices();

 protected:
    IDisplay *display_;
    VListWidget choices_;
};

class ActivityPausedPresenter : public PagePresenter, public KeypadListener {
 public:
    ActivityPausedPresenter(IDisplay *display, IEventDispatcher *events);
    ~ActivityPausedPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

    enum Choice {
       Resume = 0,
       Save = 1,
       Discard = 2
    };



 private:
    ActivityPausedView view_;
};

}  // namespace bk
