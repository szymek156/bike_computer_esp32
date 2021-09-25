#pragma once

#include "event_dispatcher.h"
#include "root_window.h"
#include "listerers_interface.h"
#include "page_presenter.h"
#include "view/TEMPLATE_view.h"

#include <display.h>

namespace bk {
// TODO: that could be composition, instead of inheritance (strategy) consider refactor
class TEMPLATEPresenter : public PagePresenter, public KeypadListener {
 public:
    TEMPLATEPresenter(IDisplay *display, IEventDispatcher *events);
    ~TEMPLATEPresenter();

    virtual void onEnter() override;
    virtual void onLeave() override;

    virtual void onButtonPressed(const KeypadData &data) override;

 private:
    TEMPLATEView view_;
};

}  // namespace bk
