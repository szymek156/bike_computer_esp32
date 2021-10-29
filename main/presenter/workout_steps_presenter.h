#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "widgets/v_list_widget.h"

#include <display.h>

namespace bk {
class WorkoutStepsView {
 public:
    WorkoutStepsView(IDisplay *display);

    // Draw static components not changing over time
    void drawStatic();

    VListWidget& getWorkoutSteps();

 protected:
    IDisplay *display_;
    VListWidget workout_steps_;
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
