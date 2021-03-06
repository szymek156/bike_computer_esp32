#pragma once

#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/welcome_view.h"
#include "root_window.h"
#include <display.h>

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class WelcomePresenter : public PagePresenter,
                         public TimeListener,
                         public KeypadListener {
 public:
    WelcomePresenter(IDisplay *display, IEventDispatcher *events);
    ~WelcomePresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onTimeData(const TimeData &data) override;
    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    WelcomeView view_;
};

}  // namespace bk
