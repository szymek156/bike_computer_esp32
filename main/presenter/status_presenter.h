#pragma once

#include <display.h>
#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/status_view.h"
#include "root_window.h"
namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class StatusPresenter : public PagePresenter, public GNSSListener, public TimeListener {
 public:
    StatusPresenter(IDisplay *display, IEventDispatcher *events);
    ~StatusPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onGNSSData(const GNSSData &data) override;
    virtual void onTimeData(const TimeData &data) override;

 private:
    StatusView view_;
};

}  // namespace bk
