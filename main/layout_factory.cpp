#include "layout_factory.h"

#include "presenter/TEMPLATE_presenter.h"
#include "presenter/activity_splash_presenter.h"
#include "presenter/select_activity_presenter.h"
#include "presenter/stats_presenter.h"
#include "presenter/status_presenter.h"
#include "presenter/test_presenter.h"
#include "presenter/welcome_presenter.h"

#include <vector>
namespace bk {
LayoutFactory::LayoutFactory(IDisplay *display, IEventDispatcher *events)
    : display_(display),
      events_(events) {
}

StatusAndMain LayoutFactory::create() {
    auto status = std::make_shared<StatusPresenter>(display_, events_);
    auto test = std::make_shared<TestPresenter>(display_, events_);
    auto welcome = std::make_shared<WelcomePresenter>(display_, events_);

    auto activity_splash = std::make_shared<ActivitySplashPresenter>(display_, events_);
    auto select_activity = std::make_shared<SelectActivityPresenter>(
        display_,
        events_,
        std::vector<std::string>{"Running", "Cycling", "Hiking", "Indoor Cycling"});

    test->setNext(welcome);
    test->setPrevious(activity_splash);

    welcome->setNext(activity_splash);
    welcome->setPrevious(test);

    activity_splash->setNext(test);
    activity_splash->setPrevious(welcome);
    activity_splash->setMore(select_activity);

    // select_activity->setMore(WORKOUTS);
    select_activity->setLess(activity_splash);

    // Return init view
    return {std::move(status), std::move(welcome)};
}
}  // namespace bk