#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"

#include <display.h>

namespace bk {
class WorkoutStepsSplashView {
 public:
    WorkoutStepsSplashView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
};

class WorkoutStepsSplashPresenter : public PagePresenter, public KeypadListener {
 public:
    WorkoutStepsSplashPresenter(IDisplay *display, IEventDispatcher *events);
    ~WorkoutStepsSplashPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    WorkoutStepsSplashView view_;
};

}  // namespace bk
