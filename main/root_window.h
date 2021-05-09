#pragma once
#include "event_dispatcher.h"
#include "layout_factory.h"
#include "listerers_interface.h"
#include "presenter/page_presenter.h"

#include <memory>

#include <display.h>

namespace bk {
class RootWindow : public WidgetListener {
 public:
    RootWindow(IEventDispatcher *events, LayoutFactory *factory);

    virtual ~RootWindow();

    virtual void onWidgetChange(const WidgetData &data);

    void setStatusWidget(PresenterPtr status);

    void setCurrentWidget(PresenterPtr current);

 private:
    // Status widget
    PresenterPtr status_widget_;

    // Main widget
    PresenterPtr current_widget_;

    IEventDispatcher *events_;
};
}  // namespace bk
