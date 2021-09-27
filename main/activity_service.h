
#pragma once

#include <array>
#include <map>
#include <string>
#include <variant>
#include <vector>

/**
 * @brief Service for querying / setting state of current activity
 * */
class ActivityService {
 public:
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

 private:
    ActivityService() = default;

    // Keep this collection and enum in correct order,
    // because commetee chimps did not figure how to implement
    // enum to string yet.
    // And never will be, they are too busy copy pasting from boost,
    // one library per 3 years, and say that "we did it".
    const std::array<const char *, 4> ACTIVITIES = {
        "Running", "Cycling", "Hiking", "Indoor Cycling"};

    enum class Activities {
        Running = 0,
        Cycling,
        Hiking,
        IndoorCycling,
    };

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

    // TODO: keep FIT instance as a pointer, delete, when activity finished
    // create, when user hits "do it" button

    // TODO: and others ctors, assign operators -> delete,
    // bla bla bla, too lazy to write them
};