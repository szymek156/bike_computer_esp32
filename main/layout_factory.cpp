#include "layout_factory.h"

#include "presenter/stats_presenter.h"
#include "presenter/status_presenter.h"
#include "presenter/welcome_presenter.h"

namespace bk {
LayoutFactory::LayoutFactory(IDisplay *display, IEventDispatcher *events, RootWindow *root)
    : display_(display),
      events_(events),
      root_(root) {
}

StatusAndMain LayoutFactory::create() {
    auto status = std::make_shared<StatusPresenter>(display_, events_, root_);
    auto welcome = std::make_shared<WelcomePresenter>(display_, events_, root_);
    auto stats = std::make_shared<StatsPresenter>(display_, events_, root_);

    welcome->setNext(stats);
    welcome->setPrevious(stats);

    stats->setNext(welcome);
    stats->setPrevious(welcome);

    return {std::move(status), std::move(welcome)};
}
}  // namespace bk