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
#include "fit.h"
#include "fit_example.h"
#include "stdio.h"

#include <cstring>
#include <cmath>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

// Mapping FIT_MESG_ID -> local_message id
static u_char local_msgs[FIT_MESGS] = {};

// Needed for conversion from UNIX time to FIT time
static const time_t system_time_offset = 631065600;

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
#define ADD_MESSAGE(NAME, LAMBDA)                                         \
    {                                                                     \
        FIT_##NAME##_MESG the_mesg;                                       \
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_##NAME], &the_mesg);          \
        [&] LAMBDA();                                                     \
                                                                          \
        auto local = local_msgs[FIT_MESG_##NAME];                         \
        if (local == 0) {                                                 \
            ADD_MESSAGE_DEF(NAME, local);                                 \
        }                                                                 \
                                                                          \
        fit_file_.writeMessage(local, &the_mesg, FIT_##NAME##_MESG_SIZE); \
    }

FITActivity::FITActivity(IEventDispatcher *events) : events_(events) {
    // Set all to 0
    memset(local_msgs, 0, FIT_MESGS);
    // 0 is reserved for rare messages
    // Add here message which happens to show up often
    local_msgs[FIT_MESG_RECORD] = 1;
    local_msgs[FIT_MESG_EVENT] = 2;
    local_msgs[FIT_MESG_LAP] = 3;
    local_msgs[FIT_MESG_SESSION] = 4;
    local_msgs[FIT_MESG_ACTIVITY] = 5;
    // max 16 local messages can be defined
}

void FITActivity::onGNSSData(const GNSSData &data) {
    // TODO: accumulate distance
    // create record message and write it
    if (data.fix_status == GNSSData::noFix) {
        return;
    }

    float distance = calculateHaversine(data);

    ESP_LOGI(TAG, "Calculated distance %f", distance);

    FIT_RECORD_MESG the_mesg;
    ADD_MESSAGE(RECORD, {
        if (data.fix_status > GNSSData::fix2d) {
            the_mesg.altitude = data.altitude;
        }

        the_mesg.position_long = data.longitude;
        the_mesg.position_lat = data.latitude;

        the_mesg.speed = data.speed_kmh;

        // Azimuth
        // data.track_degrees

        // the_mesg.distance // accumulative distance
        // the_mesg.timestamp
    })
}

void FITActivity::start() {
    ESP_LOGI(TAG, "Starting activity");

    addPrelude();
    events_->subForGNSS(this);
}

void FITActivity::pause() {
    ESP_LOGI(TAG, "Activity paused");

    ADD_MESSAGE(EVENT, {
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_STOP;
    });
}

void FITActivity::resume() {
    ESP_LOGI(TAG, "Activity resumed");

    ADD_MESSAGE(EVENT, {
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_START;
    });
}

void FITActivity::stop() {
    ESP_LOGI(TAG, "Stopping activity");

    events_->unSubForGNSS(this);

    ADD_MESSAGE(EVENT, {
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_STOP;
    });
}

void FITActivity::addPrelude() {
    ESP_LOGI(TAG, "Writing a prelude");

    // Create file prelude, start listening
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
        // Set FIT time from UTC time
        time_t now = time(0);

        struct tm utc_tm;
        // Convert local to UTC
        gmtime_r(&now, &utc_tm);
        // Convert UTC tm, to UTC time_t
        time_t utc_time = mktime(&utc_tm);
        the_mesg.time_created = utc_time - system_time_offset;

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
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_START;
    });

    ESP_LOGI(TAG, "Writing prelude finished");
}

float FITActivity::calculateHaversine(const GNSSData &data) {
    if (previous_point_.fix_status == GNSSData::noFix) {
        previous_point_ = data;

        return 0;
    }

    const float EARTH_RADIUS_KM = 6371;
    const float RADIANS = M_PI / 180.0;

    float rad_lat = (data.latitude - previous_point_.latitude) * RADIANS;
    float rad_lon = (data.longitude - previous_point_.longitude) * RADIANS;

    float a = sin(rad_lat / 2) * sin(rad_lat / 2) + sin(rad_lon / 2) * sin(rad_lon / 2) *
                                                      cos(previous_point_.latitude * RADIANS) *
                                                      cos(data.latitude * RADIANS);

    float c = 2 * atan2(sqrt(a), sqrt(1 - a));

    float distance = EARTH_RADIUS_KM * c;

    previous_point_ = data;

    return distance;
}
}  // namespace bk