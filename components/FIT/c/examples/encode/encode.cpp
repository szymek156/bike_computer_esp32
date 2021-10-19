////////////////////////////////////////////////////////////////////////////////
// The following FIT Protocol software provided may be used with FIT protocol
// devices only and remains the copyrighted property of Dynastream Innovations
// Inc. The software is being provided on an "as-is" basis and as an
// accommodation, and therefore all warranties, representations, or guarantees
// of any kind (whether express, implied or statutory) including, without
// limitation, warranties of merchantability, non-infringement, or fitness for a
// particular purpose, are specifically disclaimed.
//
// Copyright 2008-2015 Dynastream Innovations Inc.
////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include "fit_crc.h"
#include "fit_product.h"
#include "stdio.h"
#include "string.h"

#include <time.h>


static FIT_UINT16 data_crc;

void WriteData(const void *data, size_t data_size, FILE *fp);


void WriteFileHeader(FILE *fp) {
    FIT_FILE_HDR file_header;

    file_header.header_size = FIT_FILE_HDR_SIZE;
    file_header.profile_version = FIT_PROFILE_VERSION;
    file_header.protocol_version = FIT_PROTOCOL_VERSION_20;
    memcpy((FIT_UINT8 *)&file_header.data_type, ".FIT", 4);
    fseek(fp, 0, SEEK_END);
    file_header.data_size = ftell(fp) - FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16);
    file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));

    fseek(fp, 0, SEEK_SET);
    fwrite((void *)&file_header, 1, FIT_FILE_HDR_SIZE, fp);
}

void WriteMessageDefinition(FIT_UINT8 local_mesg_number,
                            const void *mesg_def_pointer,
                            size_t mesg_def_size,
                            FILE *fp) {
    FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT;
    WriteData(&header, FIT_HDR_SIZE, fp);
    WriteData(mesg_def_pointer, mesg_def_size, fp);
}

void WriteMessageDefinitionWithDevFields(FIT_UINT8 local_mesg_number,
                                         const void *mesg_def_pointer,
                                         size_t mesg_def_size,
                                         FIT_UINT8 number_dev_fields,
                                         FIT_DEV_FIELD_DEF *dev_field_definitions,
                                         FILE *fp) {
    FIT_UINT16 i;
    FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT | FIT_HDR_DEV_DATA_BIT;
    WriteData(&header, FIT_HDR_SIZE, fp);
    WriteData(mesg_def_pointer, mesg_def_size, fp);

    WriteData(&number_dev_fields, sizeof(FIT_UINT8), fp);
    for (i = 0; i < number_dev_fields; i++) {
        WriteData(&dev_field_definitions[i], sizeof(FIT_DEV_FIELD_DEF), fp);
    }
}

void WriteMessage(FIT_UINT8 local_mesg_number,
                  const void *mesg_pointer,
                  size_t mesg_size,
                  FILE *fp) {
    WriteData(&local_mesg_number, FIT_HDR_SIZE, fp);
    WriteData(mesg_pointer, mesg_size, fp);
}

void WriteDeveloperField(const void *data, size_t data_size, FILE *fp) {
    WriteData(data, data_size, fp);
}

void WriteData(const void *data, size_t data_size, FILE *fp) {
    size_t offset;

    fwrite(data, 1, data_size, fp);

    for (offset = 0; offset < data_size; offset++)
        data_crc = FitCRC_Get16(data_crc, *((FIT_UINT8 *)data + offset));
}


#define ADD_MESSAGE(NAME, LOCAL_MESG_NUMBER, LAMBDA)                                            \
    {                                                                                           \
        FIT_##NAME##_MESG the_mesg;                                                             \
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_##NAME], &the_mesg);                                \
        [&] LAMBDA();                                                                           \
                                                                                                \
        /* TODO: add definition only if needed */                                               \
        WriteMessageDefinition(                                                                 \
            LOCAL_MESG_NUMBER, fit_mesg_defs[FIT_MESG_##NAME], FIT_##NAME##_MESG_DEF_SIZE, fp); \
        WriteMessage(LOCAL_MESG_NUMBER, &the_mesg, FIT_##NAME##_MESG_SIZE, fp);                 \
    }

void example_activity() {
    // Needed for conversion from UNIX time to FIT time
    static const FIT_DATE_TIME systemTimeOffset = 631065600;

    FILE *fp;

    data_crc = 0;
    fp = fopen("example_activity.fit", "w+b");

    WriteFileHeader(fp);

    // FILE_ID is expected to be the first message in the file
    ADD_MESSAGE(FILE_ID, 0, {
        // Set FIT time from UTC time
        time_t now = time(0);

        struct tm utc_tm;
        // Convert local to UTC
        gmtime_r(&now, &utc_tm);
        // Convert UTC tm, to UTC time_t
        time_t utc_t = mktime(&utc_tm);
        the_mesg.time_created = utc_t - systemTimeOffset;

        strcpy(the_mesg.product_name, "my_esp32_bc_proto");

        the_mesg.type = FIT_FILE_ACTIVITY;
        the_mesg.manufacturer = FIT_MANUFACTURER_DEVELOPMENT;
    });

    const FIT_UINT8 appId[] = {
        0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

    ADD_MESSAGE(DEVELOPER_DATA_ID, 0, {
        the_mesg.developer_data_index = 0;
        memcpy(the_mesg.application_id, appId, FIT_DEVELOPER_DATA_ID_MESG_APPLICATION_ID_COUNT);
        the_mesg.manufacturer_id = FIT_MANUFACTURER_DEVELOPMENT;
    });

    ADD_MESSAGE(EVENT, 0, {
        the_mesg.event = FIT_EVENT_TIMER;
        the_mesg.event_type = FIT_EVENT_TYPE_START;
    });

    // FIT_RECORD_MESG the_mesg;
    ADD_MESSAGE(RECORD,
                0,
                {

                    // the_mesg.altitude
                    // the_mesg.speed
                    // the_mesg.heart_rate
                    // the_mesg.cadence
                    // the_mesg.distance // accumulative distance
                    // the_mesg.timestamp
                })

    // FIT_LAP_MESG the_mesg;
    // Summary message, represent laps, of intervals inside the session
    ADD_MESSAGE(LAP,
                0,
                {

                })

    // Additional to that, events like:
    // #define FIT_EVENT_HR_HIGH_ALERT
    // #define FIT_EVENT_HR_LOW_ALERT
    // #define FIT_EVENT_SPEED_HIGH_ALERT
    // #define FIT_EVENT_SPEED_LOW_ALERT
    // #define FIT_EVENT_CAD_HIGH_ALERT
    // #define FIT_EVENT_CAD_LOW_ALERT
    // #define FIT_EVENT_RECOVERY_HR
    // #define FIT_EVENT_BATTERY_LOW
    // #define FIT_EVENT_DISTANCE_DURATION_ALERT  // 10k reached

    // FIT_SESSION_MESG the_mesg;

    // Summary message type
    ADD_MESSAGE(SESSION, 0, {
        the_mesg.timestamp = 0;
        the_mesg.start_time = 0;
        the_mesg.start_position_lat = 0;
        the_mesg.start_position_long = 0;
        the_mesg.total_elapsed_time = 0;  // Includes pauses
        the_mesg.total_timer_time = 0;    // Excludes pauses
        the_mesg.total_distance = 0;

        // Rectangle of the area, session had place
        the_mesg.nec_lat = 0;
        the_mesg.nec_long = 0;
        the_mesg.swc_lat = 0;
        the_mesg.swc_long = 0;

        the_mesg.avg_cadence = 0;
        the_mesg.avg_speed = 0;
        the_mesg.avg_heart_rate = 0;
        // ...

        the_mesg.num_laps = 0;

        the_mesg.sport = FIT_SPORT_RUNNING;
        the_mesg.sub_sport = FIT_SUB_SPORT_GENERIC;
    });

    // FIT_ACTIVITY_MESG mesg;
    // Number of sessions is known at the end of the activity, hence
    // this message is most likely to be the last message in the file.
    ADD_MESSAGE(ACTIVITY, 0, {
        // TODO: ???
        // Start of the activity in FIT epoch
        // FIT_DATE_TIME timestamp;
        // Total activity duration
        // FIT_UINT32 total_timer_time;
        // FIT timestamp in Warsaw timezone (+ 2 hours)
        // FIT_LOCAL_DATE_TIME local_timestamp;
        // event = FIT_EVENT_ACTIVITY
        // event_type = FIT_EVENT_TYPE_STOP

        the_mesg.num_sessions = 2;
    });

    // Write CRC.
    fwrite(&data_crc, 1, sizeof(FIT_UINT16), fp);

    // Update file header with data size.
    WriteFileHeader(fp);

    fclose(fp);
}

int main(void) {
    example_activity();

    return 0;
}
