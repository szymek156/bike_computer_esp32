#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/WorkoutSteps_view.h"

#include <display.h>

namespace bk {
class WorkoutStepsView {
 public:
    WorkoutStepsView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

 protected:
    IDisplay *display_;
};

class WorkoutStepsPresenter : public PagePresenter, public KeypadListener {
 public:
    WorkoutStepsPresenter(IDisplay *display, IEventDispatcher *events);
    ~WorkoutStepsPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    WorkoutStepsView view_;
};

}  // namespace bk
