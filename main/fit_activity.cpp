#include "fit_activity.h"

// I decided to use C API instead of C++ API for following reasons:
// C API is extremely small, moreover, it is possible to fine tune
// exactly which messages, and even which fields will be part of the
// compilation - currently, that's not the case for C++.
// Unfortunately it's also very low-level, every single step of the
// protocol must be defined by developer, and by C nature itself,
// it's very repetitive.
// C++ is more abstracted, and looks very nice to use, but it
// brings a lot of code that would be not used in this application.
// There is no way to define which messages are included and which not.
// It has whole machinery for publisher/subscriber pattern for message
// decoding.
// All of that could be reduced in size, but by hand.
// Currently code generation from .csv file works only for C API...
// As long as I will not be insanely pissed off by C API,
// I will stick to it. In case if my angrymeter explodes, switching
// to the C++ API sounds like a plan B, then as an incremental step
// is to cut unused part of the code manually.
// Example encode binary from C API is 39kB
// Example encode binary from C++ API if 2.9 MB - that may not fit
// on the flash!
// Also I need to save some space in FLASH and RAM for:
// - Bluetooth stack
// - ANT driver
// - Activities, other FIT files
// - Other libraries (hopefully there should be none?), maybe timezone as
// a stretch goal
#include "activity_service.h"
#include "fit.h"
#include "fit_example.h"
#include "stdio.h"

#include <cmath>
#include <cstring>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

// Mapping FIT_MESG_ID -> local_message id
static u_char local_msgs[FIT_MESGS] = {};

// Needed for conversion from UNIX time to FIT time
static const time_t system_time_offset = 631065600;

// Reserved index for messages, which do not have prior definition written in the FIT file
static const u_char NOT_SET = 0;

#define ADD_MESSAGE_DEF(NAME, LOCAL_MESG_NUMBER)                                            \
    {                                                                                       \
        fit_file_.writeMessageDefinition(                                                   \
            LOCAL_MESG_NUMBER, fit_mesg_defs[FIT_MESG_##NAME], FIT_##NAME##_MESG_DEF_SIZE); \
    }

// This macro hides C API boilerplate. Repetitive stuff that
// is needed to create every single record in the FIT file.
// Firstly it initializes a message struct with default (invalid) data,
// Then "calls" user defined initialization,
// Finally, writes mesg definition and a message itself to the file.
// Lambdas uses operator () which is implicitly inlined,
// meaning there should be no overhead for calling it.
// Macros are not pretty, and never been, but here they save the developer from
// copy pasting error, and that's already something!
#define ADD_MESSAGE(NAME, LAMBDA)                                             \
    {                                                                         \
        FIT_##NAME##_MESG the_mesg;                                           \
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_##NAME], &the_mesg);              \
        [&] LAMBDA();                                                         \
                                                                              \
        /* If local is zero - means there is no definition, so add it      */ \
        /* This is not the optimal solution, consider 2 messages which     */ \
        /* does not have local_msg, then definition will be written every  */ \
        /* time msg is used. Something like LRU should be used, but that's */ \
        /* a stretch goal, current solution is simple, and good enough.    */ \
        auto local = local_msgs[FIT_MESG_##NAME];                             \
        if (local == NOT_SET) {                                               \
            ADD_MESSAGE_DEF(NAME, local);                                     \
        }                                                                     \
                                                                              \
        fit_file_.writeMessage(local, &the_mesg, FIT_##NAME##_MESG_SIZE);     \
    }

FITActivity::FITActivity(IEventDispatcher *events) : events_(events) {
    // Set all to NOT_SET
    memset(local_msgs, NOT_SET, FIT_MESGS);
    // NOT_SET is reserved for rare messages
    // Add here message which happens to show up often
    local_msgs[FIT_MESG_RECORD] = 1;
    local_msgs[FIT_MESG_EVENT] = 2;
    local_msgs[FIT_MESG_LAP] = 3;
    local_msgs[FIT_MESG_SESSION] = 4;
    local_msgs[FIT_MESG_ACTIVITY] = 5;
    // max 16 local messages can be defined
}

void FITActivity::onGNSSData(const GNSSData &data) {
    // create record message and write it
    if (data.fix_status == GNSSData::noFix) {
        return;
    }

    ADD_MESSAGE(RECORD, {
        the_mesg.timestamp = getFITTimestamp();

        // FIT expects to have lat/long in semicircle units
        the_mesg.position_lat = GNSSData::toSemiCircles(data.latitude);
        the_mesg.position_long = GNSSData::toSemiCircles(data.longitude);

        // accumulative distance
        the_mesg.distance = 100 * current_session_.total_distance;

        if (data.fix_status > GNSSData::fix2d) {
            the_mesg.altitude = 5 * data.altitude + 500;
        }

        // TODO: Look for float -> uint scaling reference
        // /FIT_SDK/cpp/fit_field_base.cpp void FieldBase::SetFLOAT64Value

        the_mesg.speed = 1000 * data.speed_ms;
    });

    updateByGNSS(data);

    events_->activityDataEvent(ActivityData{.lap_distance = current_lap_.total_distance,
                                            .total_distance = current_session_.total_distance});
}

void FITActivity::updateByGNSS(const GNSSData &data) {
    float distance = calculateHaversine(data);
    ESP_LOGV(TAG, "Calculated distance %f", distance);

    // TODO: set start_position_lat/long
    current_lap_.total_distance += distance;
    current_lap_.n_samples++;
    current_lap_.avg_speed += data.speed_ms;

    if (current_lap_.total_distance > 1000) {
        storeLap();
    }

    current_session_.total_distance += distance;
}

uint32_t FITActivity::getFITTimestamp() {
    // Set FIT time from UTC time
    time_t now = time(0);

    struct tm utc_tm;
    // Convert local to UTC
    gmtime_r(&now, &utc_tm);
    // Convert UTC tm, to UTC time_t
    time_t utc_time = mktime(&utc_tm);

    // Move to FIT epoch
    return utc_time - system_time_offset;
}

void FITActivity::storeLap() {
    auto timestamp = getFITTimestamp();
    current_lap_.lap.timestamp = timestamp;

    current_lap_.lap.end_position_lat = GNSSData::toSemiCircles(last_point_.latitude);
    current_lap_.lap.end_position_long = GNSSData::toSemiCircles(last_point_.longitude);

    current_lap_.lap.total_elapsed_time = timestamp - current_lap_.lap.start_time;
    // TODO: support pausing
    current_lap_.lap.total_timer_time = timestamp - current_lap_.lap.start_time;

    current_lap_.lap.total_distance = 100 * current_lap_.total_distance;

    // Store float to uint
    current_lap_.avg_speed /= current_lap_.n_samples;
    current_lap_.lap.avg_speed = 1000 * current_lap_.avg_speed;

    current_lap_.lap.sport = getFITSport();
    current_lap_.lap.sub_sport = getFITSubSport();

    fit_file_.writeMessage(local_msgs[FIT_MESG_LAP], &current_lap_.lap, FIT_LAP_MESG_SIZE);

    // Increase number of laps
    current_session_.session.num_laps++;
    current_lap_ = Lap();
}

void FITActivity::storeSession() {
    current_session_.session.timestamp = getFITTimestamp();
    current_session_.session.sport = getFITSport();
    current_session_.session.sub_sport = getFITSubSport();
    // TODO: that should be dynamic
    current_session_.session.trigger = FIT_SESSION_TRIGGER_ACTIVITY_END;

    fit_file_.writeMessage(
        local_msgs[FIT_MESG_SESSION], &current_session_.session, FIT_SESSION_MESG_SIZE);

    current_activity_.activity.num_sessions++;
    current_session_ = Session();
}
void FITActivity::storeActivity() {
    current_activity_.activity.timestamp = getFITTimestamp();
    current_activity_.activity.type = FIT_ACTIVITY_MANUAL;

    fit_file_.writeMessage(
        local_msgs[FIT_MESG_ACTIVITY], &current_activity_.activity, FIT_ACTIVITY_MESG_SIZE);

    current_activity_ = Activity();
}

void FITActivity::start() {
    ESP_LOGI(TAG, "Starting activity");

    auto ts = getFITTimestamp();
    current_session_.session.start_time = ts;
    current_lap_.lap.start_time = ts;

    addPrelude();
    events_->subForGNSS(this);
}

void FITActivity::pause() {
    ESP_LOGI(TAG, "Activity paused");

    ADD_MESSAGE(EVENT, {
        the_mesg.timestamp = getFITTimestamp();
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_STOP;
    });
}

void FITActivity::resume() {
    ESP_LOGI(TAG, "Activity resumed");

    ADD_MESSAGE(EVENT, {
        the_mesg.timestamp = getFITTimestamp();
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_START;
    });
}

void FITActivity::stop() {
    ESP_LOGI(TAG, "Stopping activity");

    events_->unSubForGNSS(this);

    ADD_MESSAGE(EVENT, {
        the_mesg.timestamp = getFITTimestamp();
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_STOP;
    });

    storeLap();
    storeActivity();
    storeSession();
}

void FITActivity::addPrelude() {
    ESP_LOGI(TAG, "Writing a prelude");

    // Create file prelude
    // Add message definition to often used messages at the beginning of the file
    // That's the best practice.
    ADD_MESSAGE_DEF(RECORD, local_msgs[FIT_MESG_RECORD]);
    ADD_MESSAGE_DEF(EVENT, local_msgs[FIT_MESG_EVENT]);
    ADD_MESSAGE_DEF(LAP, local_msgs[FIT_MESG_LAP]);
    ADD_MESSAGE_DEF(SESSION, local_msgs[FIT_MESG_SESSION]);
    ADD_MESSAGE_DEF(ACTIVITY, local_msgs[FIT_MESG_ACTIVITY]);

    // Other messages will use local_message == 0, definition will be added ad-hoc
    // just before message use

    // FILE_ID is expected to be the first message in the file
    ADD_MESSAGE(FILE_ID, {
        the_mesg.time_created = getFITTimestamp();

        strcpy(the_mesg.product_name, "my_esp32_bc_proto");

        the_mesg.type = FIT_FILE_ACTIVITY;
        the_mesg.manufacturer = FIT_MANUFACTURER_DEVELOPMENT;
    });

    const FIT_UINT8 appId[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

    ADD_MESSAGE(DEVELOPER_DATA_ID, {
        the_mesg.developer_data_index = 0;
        memcpy(the_mesg.application_id, appId, FIT_DEVELOPER_DATA_ID_MESG_APPLICATION_ID_COUNT);
        the_mesg.manufacturer_id = FIT_MANUFACTURER_DEVELOPMENT;
    });

    // Here we go!
    ADD_MESSAGE(EVENT, {
        the_mesg.timestamp = getFITTimestamp();
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_START;
    });

    ESP_LOGI(TAG, "Writing prelude finished");
}

float FITActivity::calculateHaversine(const GNSSData &data) {
    if (last_point_.fix_status == GNSSData::noFix) {
        last_point_ = data;

        return 0;
    }

    const float EARTH_RADIUS_KM = 6371;
    const float RADIANS = M_PI / 180.0;

    float rad_lat = (data.latitude - last_point_.latitude) * RADIANS;
    float rad_lon = (data.longitude - last_point_.longitude) * RADIANS;

    float a = sin(rad_lat / 2) * sin(rad_lat / 2) + sin(rad_lon / 2) * sin(rad_lon / 2) *
                                                        cos(last_point_.latitude * RADIANS) *
                                                        cos(data.latitude * RADIANS);

    float c = 2 * atan2(sqrt(a), sqrt(1 - a));

    float distance = EARTH_RADIUS_KM * c;

    last_point_ = data;

    return distance;
}

FIT_SPORT FITActivity::getFITSport() {
    using Activities = ActivityService::Activities;

    // TODO: calling activity service here is circular dependency
    // I don't quite like it
    auto activity = ActivityService::instance().getCurrentActivity();

    switch (activity) {
        case Activities::Running: {
            return FIT_SPORT_RUNNING;
        }
        case Activities::Cycling: {
            return FIT_SPORT_CYCLING;
        }
        case Activities::Hiking: {
            return FIT_SPORT_HIKING;
        }
        case Activities::IndoorCycling: {
            return FIT_SPORT_CYCLING;
        }
        case Activities::NordicWalking: {
            return FIT_SPORT_RUNNING;
        }
    }

    ESP_LOGE(TAG, "Invalid activity received for sport %d", (int)activity);

    return FIT_SPORT_INVALID;
}

FIT_SUB_SPORT FITActivity::getFITSubSport() {
    using Activities = ActivityService::Activities;

    auto activity = ActivityService::instance().getCurrentActivity();

    switch (activity) {
        case Activities::Running: {
            return FIT_SUB_SPORT_GENERIC;
        }
        case Activities::Cycling: {
            return FIT_SUB_SPORT_GENERIC;
        }
        case Activities::Hiking: {
            return FIT_SUB_SPORT_GENERIC;
        }
        case Activities::IndoorCycling: {
            return FIT_SUB_SPORT_INDOOR_CYCLING;
        }
        case Activities::NordicWalking: {
            return FIT_SUB_SPORT_SPEED_WALKING;
        }
    }

    ESP_LOGE(TAG, "Invalid activity received for sub_sport %d", (int)activity);
    return FIT_SUB_SPORT_INVALID;
}

}  // namespace bk