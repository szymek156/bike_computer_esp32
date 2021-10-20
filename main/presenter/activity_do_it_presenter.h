#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/activity_do_it_view.h"
#include "layout_factory.h"

#include <display.h>

namespace bk {
class ActivityDoItPresenter : public PagePresenter, public KeypadListener, public GNSSListener {
 public:
    ActivityDoItPresenter(IDisplay *display, IEventDispatcher *events);
    ~ActivityDoItPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;
    virtual void onGNSSData(const GNSSData &data) override;


 private:
    ActivityDoItView view_;
    ActivityLayoutFactory layout_factory_;
};

}  // namespace bk
