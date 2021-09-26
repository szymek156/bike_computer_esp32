
#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

/**
 * @brief Service for querying / setting state of current activity
 * */
class ActivityService {
 public:
    static ActivityService &getService();

    // Set and get strings, not enums, because this is what
    // consumers (presenters) expect. It's easier to work
    // with strings for those classes. Enum to str conversion
    // is used inside this service.
    void setCurrentActivityType(const std::string &activity);
    void setActivityWorkout(const std::string &workout);

    std::string getCurrentActivityType();
    std::string getCurrentActivityWorkout();

    std::vector<std::string> getActivities();
    std::vector<std::string> getWorkouts(const std::string &activity);

    std::string getCurrentWorkoutDescription();

 private:
    ActivityService() = default;

    const std::vector<std::string> ACTIVITIES = {"Running", "Cycling", "Hiking", "Indoor Cycling"};

    enum class Activities {
        Running,
        Cycling,
        Hiking,
        IndoorCycling,
    };

    const std::map<std::string, Activities> str_to_activities_ = {
        {"Running", Activities::Running},
        {"Cycling", Activities::Cycling},
        {"Hiking", Activities::Hiking},
        {"Indoor Cycling", Activities::IndoorCycling},
    };

    enum class RunningWorkouts {
        _5k,
        _10k,
        HalfMarathon,
        Marathon,
        CooperTest

    };

    const std::map<std::string, RunningWorkouts> str_to_running_workouts_ = {
        {"5k", RunningWorkouts::_5k},
        {"10k", RunningWorkouts::_10k},
        {"HalfMarathon", RunningWorkouts::HalfMarathon},
        {"Marathon", RunningWorkouts::Marathon},
        {"CooperTest", RunningWorkouts::CooperTest},
    };

    enum class CyclingWorkouts {

    };

    Activities current_activity_type_;
    std::variant<RunningWorkouts, CyclingWorkouts> current_workout_type_;

    // Inefficient? Thank those idiots from the cometee to not
    // give enum to str capabilities to the standard
    // for over 40 years!
    // Meanwhile in Rust:
    // #derive[Debug] // drops the mic
    // const std::map<std::string, Activities> str_to_activities_ = {
    //     {"Running", Activities::Running},
    //     {"Cycling", Activities::Cycling},
    //     {"Hiking", Activities::Hiking},
    //     {"Indoor Cycling", Activities::IndoorCycling},
    // };

    std::string enum2str(Activities e);
    std::string enum2str(RunningWorkouts e);
    std::string enum2str(CyclingWorkouts e);

    // TODO: keep FIT instance as a pointer, delete, when activity finished
    // create, when user hits "do it" button

    // TODO: and others ctors, assign operators -> delete,
    // bla bla bla, too lazy to write them
};