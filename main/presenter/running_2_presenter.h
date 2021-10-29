#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/running_2_view.h"

#include <display.h>

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class Running2Presenter : public PagePresenter, public KeypadListener, public ActivityDataListener {
 public:
    Running2Presenter(IDisplay *display, IEventDispatcher *events);
    ~Running2Presenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

    virtual void onActivityData(const ActivityData &data) override;

 private:
    Running2View view_;
};

}  // namespace bk
