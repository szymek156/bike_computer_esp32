#pragma once

#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "root_window.h"
#include "v_list_presenter.h"
#include "view/select_activity_view.h"

#include <display.h>

namespace bk {
class SelectActivityPresenter : public PagePresenter, public KeypadListener {
 public:
    SelectActivityPresenter(IDisplay *display,
                            IEventDispatcher *events,
                            const std::vector<std::string> &activities);
    ~SelectActivityPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    SelectActivityView view_;
    VListPresenter activities_presenter_;
};

}  // namespace bk
