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
    auto status = std::make_shared<StatusPresenter>(display_, events_);
    auto welcome = std::make_shared<WelcomePresenter>(display_, events_);
    auto stats = std::make_shared<StatsPresenter>(display_, events_);

    welcome->setNext(stats);
    welcome->setPrevious(stats);

    stats->setNext(welcome);
    stats->setPrevious(welcome);

    return {std::move(status), std::move(welcome)};
}
}  // namespace bk