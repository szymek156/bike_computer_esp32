#pragma once
#include "event_dispatcher.h"
#include "presenter/page_presenter.h"

#include <memory>
#include <utility>

#include <display.h>

namespace bk {
// Fuck you C++
using StatusAndMain = std::pair<std::unique_ptr<IPagePresenter>, std::unique_ptr<IPagePresenter>>;

class LayoutFactory {
 public:
    LayoutFactory(IDisplay *display, IEventDispatcher *events);
    virtual ~LayoutFactory() = default;

    StatusAndMain create();

    IDisplay *display_;
    IEventDispatcher *events_;
};

}  // namespace bk