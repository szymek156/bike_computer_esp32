#pragma once

#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "root_window.h"
#include "view/stats_view.h"

#include <display.h>

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class StatsPresenter : public PagePresenter, public KeypadListener {
 public:
    StatsPresenter(IDisplay *display, IEventDispatcher *events);
    ~StatsPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    StatsView view_;
};

}  // namespace bk
