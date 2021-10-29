#pragma once

#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "root_window.h"
#include "view/running_1_view.h"

#include <display.h>

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class Running1Presenter : public PagePresenter, public KeypadListener, ActivityDataListener {
 public:
    Running1Presenter(IDisplay *display, IEventDispatcher *events);
    ~Running1Presenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

    virtual void onActivityData(const ActivityData &data) override;

 private:
    Running1View view_;
};

}  // namespace bk
