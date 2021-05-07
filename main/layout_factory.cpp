#include "layout_factory.h"

#include "presenter/status_presenter.h"
#include "presenter/welcome_presenter.h"

namespace bk {
LayoutFactory::LayoutFactory(IDisplay *display, IEventDispatcher *events)
    : display_(display),
      events_(events) {
}

StatusAndMain LayoutFactory::create() {
    return {std::make_unique<StatusPresenter>(display_, events_),
            std::make_unique<WelcomePresenter>(display_, events_)};
}
}  // namespace bk