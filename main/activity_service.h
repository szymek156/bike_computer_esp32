
#pragma once

#include "fit_activity.h"
#include "event_dispatcher.h"

#include <array>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
namespace bk {

/**
 * @brief Service for querying / setting state of current activity
 * */
class ActivityService {
 public:
    enum class Activities {
        Running = 0,
        Cycling,
        Hiking,
        IndoorCycling,
    };

    static ActivityService &instance();

    // Set and get strings, not enums, because this is what
    // consumers (presenters) expect. It's easier to work
    // with strings for those classes. Enum to str conversion
    // is used inside this service.
    void setCurrentActivityType(const std::string &activity);
    void setActivityWorkout(const std::string &workout);

    std::string getCurrentActivityType();
    std::string getCurrentActivityWorkout();

    std::vector<std::string> getActivities();
    std::vector<std::string> getWorkouts();

    std::string getCurrentWorkoutDescription();

    Activities getCurrentActivity();

    /** @brief New FIT activity is created.
     */
    void newActivity();

    /** @brief Activity is removed from the storage
     */
    void discardActivity();

    /** @brief Starts data recording
     */
    void startActivity();

    /** @brief Adds handle to the EventDispatcher */
    void setEventDispatcher(IEventDispatcher *events);

 private:
    ActivityService() = default;

    // Keep this collection and enum in correct order,
    // because commetee chimps did not figure how to implement
    // enum to string yet.
    // And never will be, they are too busy copy pasting from boost,
    // one library per 3 years, and say that "we did it".
    const std::array<const char *, 4> ACTIVITIES = {
        "Running", "Cycling", "Hiking", "Indoor Cycling"};

    // Compiler has no idea this has 5 elements, you need to write it!
    // auto of course does not work here too!
    // Class template arg deduction from C++17 - same here!
    // I made it an array just for fun, now I regret.
    const std::array<const char *, 5> RUNNING_WORKOUTS = {
        "5k", "10k", "Half Marathon", "Marathon", "Cooper Test"};

    enum class RunningWorkouts {
        _5k = 0,
        _10k,
        HalfMarathon,
        Marathon,
        CooperTest

    };

    const std::vector<std::string> CYCLING_WORKOUTS = {"CycWorkout1", "CycWorkout2"};
    enum class CyclingWorkouts { CycWorkout1, CycWorkout2 };

    Activities current_activity_type_;
    std::variant<RunningWorkouts, CyclingWorkouts> current_workout_type_;

    std::unique_ptr<FITActivity> fit_activity_;

    IEventDispatcher *events_;
    // TODO: and others ctors, assign operators -> delete,
    // bla bla bla, too lazy to write them
};
}  // namespace bk