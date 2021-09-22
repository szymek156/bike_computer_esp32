#pragma once

#include "event_dispatcher.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "root_window.h"
#include "view/select_activity_view.h"

#include <string>
#include <vector>

#include <display.h>

namespace bk {
class VListPresenter : public PagePresenter, public KeypadListener {
 public:
    VListPresenter(IDisplay *display,
                   IEventDispatcher *events,
                   const std::vector<std::string> &elements);
    ~VListPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    SelectActivityView view_;
    std::vector<std::string> elements_;
};

}  // namespace bk
