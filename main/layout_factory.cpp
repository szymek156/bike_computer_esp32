#include "layout_factory.h"

#include "activity_service.h"
#include "presenter/TEMPLATE_presenter.h"
#include "presenter/activity_selected_presenter.h"
#include "presenter/activity_splash_presenter.h"
#include "presenter/activity_workouts_presenter.h"
#include "presenter/select_activity_presenter.h"
#include "presenter/stats_presenter.h"
#include "presenter/stats_selected_presenter.h"
#include "presenter/stats_splash_presenter.h"
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
    auto select_activity = std::make_shared<SelectActivityPresenter>(display_, events_);

    auto activity_workouts = std::make_shared<ActivityWorkoutsPresenter>(display_, events_);

    auto activity_selected = std::make_shared<ActivitySelectedPresenter>(display_, events_);

    auto stats_splash = std::make_shared<StatsSplashPresenter>(display_, events_);
    auto select_stats = std::make_shared<StatsPresenter>(display_, events_);
    auto stats_summary = std::make_shared<StatsSelectedPresenter>(display_, events_);

    test->setNext(welcome);
    test->setPrevious(activity_splash);

    welcome->setNext(activity_splash);
    welcome->setPrevious(test);

    activity_splash->setNext(stats_splash);
    activity_splash->setPrevious(welcome);
    activity_splash->setMore(select_activity);

    select_activity->setMore(activity_workouts);
    select_activity->setLess(activity_splash);

    activity_workouts->setMore(activity_selected);
    activity_workouts->setLess(select_activity);

    // activity_selected->setMore(activity_get_ready);
    activity_selected->setLess(activity_workouts);

    stats_splash->setNext(test);
    stats_splash->setPrevious(activity_splash);
    stats_splash->setMore(select_stats);

    select_stats->setLess(stats_splash);
    select_stats->setMore(stats_summary);

    stats_summary->setLess(select_stats);

    // Return init view
    return {std::move(status), std::move(welcome)};
}
}  // namespace bk