#pragma once

#include <display.h>
#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/status_view.h"

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class StatusPresenter : public IPagePresenter, public GNSSListener, public TimeListener {
 public:
    StatusPresenter(IDisplay *display, IEventDispatcher *events);
    ~StatusPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onGNSSData(const GNSSData &data) override;
    virtual void onTimeData(const TimeData &data) override;

 private:
    StatusView view_;
    IEventDispatcher *events_;
};

}  // namespace bk
