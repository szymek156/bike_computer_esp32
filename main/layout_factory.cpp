#include "layout_factory.h"


#include <vector>

#include "presenter/select_activity_presenter.h"
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
    // auto stats = std::make_shared<StatsPresenter>(display_, events_);
    auto select_activity = std::make_shared<SelectActivityPresenter>(
        display_, events_, std::vector<std::string>{"Running", "Cycling", "Hiking", "Indoor Cycling"});

    welcome->setNext(select_activity);
    welcome->setPrevious(select_activity);

    select_activity->setNext(welcome);
    select_activity->setPrevious(welcome);

    return {std::move(status), std::move(welcome)};
}
}  // namespace bk