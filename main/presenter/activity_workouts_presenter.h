#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/activity_workouts_view.h"

#include <display.h>

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class ActivityWorkoutsPresenter : public PagePresenter, public KeypadListener {
 public:
    ActivityWorkoutsPresenter(IDisplay *display, IEventDispatcher *events);
    ~ActivityWorkoutsPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    ActivityWorkoutsView view_;
};

}  // namespace bk
