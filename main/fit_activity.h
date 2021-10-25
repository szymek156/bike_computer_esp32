#pragma once
#include "event_dispatcher.h"
#include "fit_file.h"
#include "listerers_interface.h"

namespace bk {

class FITActivity : public GNSSListener {
 public:
    FITActivity(IEventDispatcher *events);

    void onGNSSData(const GNSSData &data) override;

    /** @brief Starts data collection */
    void start();

    /** @brief Pauses data collection, but does not closes the activity */
    void pause();

    /** @brief Resumes data collection and writing to the file */
    void resume();

    /** @brief Creates final file */
    void stop();

 private:
    /** @brief Adds initial messages to the FIT file */
    void addPrelude();

   float calculateHaversine(const GNSSData &data);

    /** @brief Keeps summary data for whole Session */
    struct Session {};

    /** @brief Keeps summary data for whole Lap */
    struct Lap {};

    /** @brief Keeps summary data for whole Activity */
    struct Activity {};

    IEventDispatcher *events_;

    /** @brief Interface to writing data to the file */
    FITFile fit_file_;

    Session this_session_;
    Lap current_lap_;
    Activity current_activity_;

   GNSSData previous_point_;
    static constexpr const char *TAG = "FIT";

};
}  // namespace bk