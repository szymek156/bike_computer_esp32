#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/activity_selected_view.h"

#include <display.h>

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class ActivitySelectedPresenter : public PagePresenter, public KeypadListener {
 public:
    ActivitySelectedPresenter(IDisplay *display, IEventDispatcher *events);
    ~ActivitySelectedPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    ActivitySelectedView view_;
};

}  // namespace bk
