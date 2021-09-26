#include "activity_service.h"

ActivityService &ActivityService::getService() {
    static ActivityService current;

    return current;
}

void ActivityService::setCurrentActivityType(const std::string &activity) {
    current_activity_type_ = str_to_activities_.at(activity);
}

void ActivityService::setActivityWorkout(const std::string &workout) {
    switch (current_activity_type_)
    {
    case Activities::Running:
        current_workout_type_ = str_to_running_workouts_.at(workout);
        break;

    case Activities::Cycling:

    default:
        break;
    }
}

std::string ActivityService::getCurrentActivityType() {
    return "";
}
std::string ActivityService::getCurrentActivityWorkout() {
    return "";
}

std::vector<std::string> ActivityService::getActivities() {
    std::vector<std::string> activities;

    for [name, val] : str_to_activities_ {

    }
    return {""};
}

std::vector<std::string> ActivityService::getWorkouts(const std::string &activity) {
    return {""};
}

std::string ActivityService::getCurrentWorkoutDescription() {
    return "";
}

std::string ActivityService::enum2str(Activities e) {
    return "";
}
std::string ActivityService::enum2str(RunningWorkouts e) {
    return "";
}
std::string ActivityService::enum2str(CyclingWorkouts e) {
    return "";
}
