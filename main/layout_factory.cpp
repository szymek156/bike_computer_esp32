#include "layout_factory.h"

#include "presenter/stats_presenter.h"
#include "presenter/status_presenter.h"
#include "presenter/welcome_presenter.h"

namespace bk {
LayoutFactory::LayoutFactory(IDisplay *display, IEventDispatcher *events)
    : display_(display),
      events_(events) {
}

StatusAndMain LayoutFactory::create() {
    auto status = std::make_unique<StatusPresenter>(display_, events_);
    auto welcome = std::make_unique<WelcomePresenter>(display_, events_);
    auto stats = std::make_unique<StatsPresenter>(display_, events_);

    // TODO: shared pointers?
    // welcome->setNext()

    return {std::move(status), std::move(welcome)};
}
}  // namespace bk