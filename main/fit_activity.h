#pragma once
#include "event_dispatcher.h"
#include "fit_file.h"
#include "listerers_interface.h"

#include <fit_example.h>

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

    void updateByGNSS(const GNSSData &data);

    float calculateHaversine(const GNSSData &data);

    uint32_t getFITTimestamp();

    void storeLap();

    void storeSession();

    void storeActivity();

    IEventDispatcher *events_;

    /** @brief Interface to writing data to the file */
    FITFile fit_file_;

    struct Activity {
        Activity() {
            // By default set all fields to values "invalid" (representation set to all 1's in
            // binary). They are ignored during decoding, or used if are set to something meaningful
            Fit_InitMesg(fit_mesg_defs[FIT_MESG_ACTIVITY], &activity);
        }

        FIT_ACTIVITY_MESG activity;
    };

    // TODO: add UT - assert by default all is set to "invalid"
    struct Session {
        Session() {
            Fit_InitMesg(fit_mesg_defs[FIT_MESG_SESSION], &session);

            total_distance = 0;
        }

        float total_distance;

        FIT_SESSION_MESG session;
    };

    // Here composition over inheritance shines brighter
    struct Lap {
        Lap() {
            Fit_InitMesg(fit_mesg_defs[FIT_MESG_LAP], &lap);

            lap.total_distance = 0;
            lap.total_elapsed_time = 0;
            lap.total_timer_time = 0;

            total_distance = 0;
            avg_speed = 0;
            n_samples = 0;
        }

        // FIT uses uint to keep that value.
        // Accumulate in float, and at the end use Scale/Offset to
        // convert to uint.
        float total_distance;
        float avg_speed;
        int n_samples;

        FIT_LAP_MESG lap;
    };

    /** @brief Keeps summary data for whole Activity */
    Activity current_activity_;

    /** @brief Keeps summary data for whole Session */
    Session current_session_;

    /** @brief Keeps summary data for whole Lap */
    Lap current_lap_;

    GNSSData last_point_;
    static constexpr const char *TAG = "FIT";
};
}  // namespace bk