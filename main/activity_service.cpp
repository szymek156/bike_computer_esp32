#include "activity_service.h"

#include <algorithm>
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "fit_activity.h"

#include <esp_log.h>

namespace bk {
ActivityService &ActivityService::instance() {
    static ActivityService current;

    return current;
}

template <typename Idiots>
size_t getIdx(Idiots &collection, const std::string &key) {
    // 2 lines of code and so many things are wrong.
    // 2021, and vector still does not have find_idx method.
    // You need to use the shittiest API for collection algorithms
    // human kind ever invented. OF COURSE I WANT TO USE THE WHOLE COLLECTION!
    // WHY DO I NEED TO WRITE THIS. I NEVER USED IT DIFFERENTLY THAN begin(), end()
    // FOR TEN FUCKIN YEARS!
    // YOU FUCKIN MORONS, HOPE THIS LANGUAGE WILL BE DEAD TILL END OF THIS YEAR.
    // Everything in C++ is implicit, because fuck you, but enum class
    // is not, because, fuck you. The most common conversion in the world,
    // enum to int and back is explicit.
    // Every time you idiots add something, you fix one thing (enum scoping)
    // but break other thing (implicit conversion).
    // Next version of enum, correct one, how will be called?
    // const enum auto class union struct const? And when, C++3055?
    // Fuck you.
    // You are unable to design correctly enums, but want to implement modules?
    // Fuck you, you fucking fucks.
    auto itr = std::find(collection.cbegin(), collection.cend(), key);

    // FUCKIN NAMING, 'distance' WTF IS THAT, I JUST WANT GODDAM IDX.
    // ALSO, DISTANCE CAN BE NEGATIVE! WATCH OUT FOR ARGUMENTS ORDER.
    // HAVE YOU EVER BEEN ON ANY MATH CLASS? NEGATIVE DISTANCE?
    // YOU HAVE NEGATIVE IQ, FOR SURE.
    return std::distance(collection.cbegin(), itr);
}

void ActivityService::setCurrentActivityType(const std::string &activity) {
    current_activity_type_ = (Activities)getIdx(ACTIVITIES, activity);
}

void ActivityService::setActivityWorkout(const std::string &workout) {
    switch (current_activity_type_) {
        case Activities::Running:
            current_workout_type_ = (RunningWorkouts)getIdx(RUNNING_WORKOUTS, workout);
            break;

        case Activities::Cycling:
            current_workout_type_ = (CyclingWorkouts)getIdx(CYCLING_WORKOUTS, workout);

        default:
            break;
    }
}

std::string ActivityService::getCurrentActivityType() {
    // Why this code is even there?
    // BECAUSE IMPLEMENTING ENUM TO STRING IS TOO HARD
    // FOR C++ COMETEE IDIOTS.
    // SURE, GO AHEAD, IMPLEMENT RANGES - HA HA HA
    // Torvalds was right, he was so goddam right.
    return ACTIVITIES[(size_t)current_activity_type_];
}

// Description of crap below:
// It creates a struct from template parameters and uses them as overload
// operator(), next define deduction guide
// All of that in completely unreadable syntax for mere human.
// Finally generate a struct with lambdas inside with different arguments
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

// All of it could be achieved simply by:
// struct Visitor {
//     void operator()(A)  {}
//     void operator()(B) {}
// };
// But I just wanted to see how ugly C++ could be.
// I am satisfied.

std::string ActivityService::getCurrentActivityWorkout() {
    // Of course, there is no IDE that could handle this,
    // everything is red-squiggled
    return std::visit(overloaded{
                          [&](RunningWorkouts w) {
                              // not known conversion from const char* to string
                              // in this case (Why? Because fuck you!)
                              // - compiler splits directly to your face
                              // error message with 10 000 characters (no kidding)
                              // which is still reasonable amount while working with
                              // templates.
                              return std::string(RUNNING_WORKOUTS[(size_t)w]);
                          },
                          [&](CyclingWorkouts w) {
                              return CYCLING_WORKOUTS[(size_t)w];
                          },
                      },
                      current_workout_type_);
}

std::vector<std::string> ActivityService::getActivities() {
    return std::vector<std::string>(ACTIVITIES.cbegin(), ACTIVITIES.cend());
}

// TODO: use FIT workout file type to handle workouts, descriptions etc.
std::vector<std::string> ActivityService::getWorkouts() {
    switch (current_activity_type_) {
        case Activities::Running: {
            // No conversion from array to vector,
            // fuckin quantum magic, how on earth that could be done?
            // return RUNNING_WORKOUTS;

            // Shitness detector blinks red! - allow to compile
            return std::vector<std::string>(RUNNING_WORKOUTS.cbegin(), RUNNING_WORKOUTS.cend());
        }
        case Activities::Cycling: {
            return CYCLING_WORKOUTS;
        }

        default:
            break;
    }

    return {""};
}

std::string ActivityService::getCurrentWorkoutDescription() {
    return "";
}

ActivityService::Activities ActivityService::getCurrentActivity() {
    return current_activity_type_;
}

void ActivityService::newActivity() {
    fit_activity_.reset(new FITActivity(events_));
}

void ActivityService::discardActivity() {
    // If discard is called on nullptr - that's a bug so go investigate it!
    fit_activity_->setDiscard(true);
    fit_activity_.reset(nullptr);
}

void ActivityService::startActivity() {
    fit_activity_->start();
}

void ActivityService::pauseActivity() {
    fit_activity_->pause();
}
void ActivityService::resumeActivity() {
    fit_activity_->resume();
}
void ActivityService::storeActivity() {
    fit_activity_->stop();
    fit_activity_->setDiscard(false);
    fit_activity_.reset(nullptr);
}

void ActivityService::setEventDispatcher(IEventDispatcher *events) {
    events_ = events;
}

}  // namespace bk