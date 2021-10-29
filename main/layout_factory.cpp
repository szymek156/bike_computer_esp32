#include "layout_factory.h"

#include "activity_service.h"
#include "presenter/TEMPLATE_presenter.h"
#include "presenter/activity_do_it_presenter.h"
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
#include "presenter/running_1_presenter.h"
#include "presenter/running_2_presenter.h"
#include "presenter/workout_steps_presenter.h"

#include <vector>
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

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
    auto activity_do_it = std::make_shared<ActivityDoItPresenter>(display_, events_);

    auto stats_splash = std::make_shared<StatsSplashPresenter>(display_, events_);
    auto select_stats = std::make_shared<StatsPresenter>(display_, events_);
    auto stats_summary = std::make_shared<StatsSelectedPresenter>(display_, events_);

    auto workout_steps = std::make_shared<WorkoutStepsPresenter>(display_, events_);


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

    //This is a case where going to next widget depends on the
    // value selected from the list.
    // Here are 2 options "do it" and "view", view should go to the
    // WorkoutStepsPresenter,
    activity_selected->setMore({activity_do_it, workout_steps});
    activity_selected->setLess(activity_workouts);

    workout_steps->setLess(activity_selected);
    // setMore() - detailed description

    activity_do_it->setLess(activity_selected);
    // More - start recording, move to first page of dynamically created layout
    // Done inside activity_do_it presenter

    stats_splash->setNext(test);
    stats_splash->setPrevious(activity_splash);
    stats_splash->setMore(select_stats);

    select_stats->setLess(stats_splash);
    select_stats->setMore(stats_summary);

    stats_summary->setLess(select_stats);

    // Return init view
    return {std::move(status), std::move(welcome)};
}

ActivityLayoutFactory::ActivityLayoutFactory(IDisplay *display, IEventDispatcher *events)
    : display_(display),
      events_(events) {
}

std::shared_ptr<PagePresenter> ActivityLayoutFactory::create(ActivityService::Activities activity) {
    static const char* TAG = "ActivityLayoutFactory";

    switch (activity) {
        case ActivityService::Activities::Running: {
            auto r1 = std::make_shared<Running1Presenter>(display_, events_);
            auto r2 = std::make_shared<Running2Presenter>(display_, events_);
            auto r3 = std::make_shared<WorkoutStepsPresenter>(display_, events_);

            // TODO: this is a memleak, one of pointers should be weak
            r1->setNext(r2);
            r1->setPrevious(r3);

            r2->setNext(r3);
            r2->setPrevious(r1);

            // TODO: detailed descritpion
            // r3->setMore()
            r3->setLess(r1);

            return r1;
        }
        case ActivityService::Activities::Cycling: {
            return nullptr;
        }

        default:
            ESP_LOGE(TAG, "Unable to create a layout for activity %u", static_cast<uint32_t>(activity));
            return nullptr;
    }
}

}  // namespace bk