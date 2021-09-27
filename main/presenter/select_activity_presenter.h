#pragma once

#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "root_window.h"
#include "view/select_activity_view.h"

#include <display.h>

namespace bk {
class SelectActivityPresenter : public PagePresenter, public KeypadListener {
 public:
    SelectActivityPresenter(IDisplay *display,
                            IEventDispatcher *events);
    ~SelectActivityPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    SelectActivityView view_;
};

}  // namespace bk
