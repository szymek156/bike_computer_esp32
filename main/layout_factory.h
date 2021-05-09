#pragma once
#include "event_dispatcher.h"
#include "presenter/page_presenter.h"

#include <memory>
#include <utility>

#include <display.h>

namespace bk {
// Fuck you C++
using StatusAndMain = std::pair<std::shared_ptr<PagePresenter>, std::shared_ptr<PagePresenter>>;

class LayoutFactory {
 public:
    LayoutFactory(IDisplay *display, IEventDispatcher *events);
    virtual ~LayoutFactory() = default;

    StatusAndMain create();

    IDisplay *display_;
    IEventDispatcher *events_;
};

}  // namespace bk