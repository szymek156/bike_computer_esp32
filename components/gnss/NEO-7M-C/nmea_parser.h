// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_types.h"
#include "esp_err.h"

#define GPS_MAX_SATELLITES_IN_USE (12)
#define GPS_MAX_SATELLITES_IN_VIEW (16)

/**
 * @brief NMEA Parser runtime buffer size
 *
 */
#define NMEA_PARSER_RUNTIME_BUFFER_SIZE (512)
#define NMEA_MAX_STATEMENT_ITEM_LENGTH (16)
#define NMEA_EVENT_LOOP_QUEUE_SIZE (16)

/**
 * @brief GPS fix type
 *
 */
typedef enum {
    GPS_FIX_INVALID, /*!< Not fixed */
    GPS_FIX_GPS,     /*!< GPS */
    GPS_FIX_DGPS,    /*!< Differential GPS */
} gps_fix_t;

/**
 * @brief GPS fix mode
 *
 */
typedef enum {
    GPS_MODE_INVALID = 1, /*!< Not fixed */
    GPS_MODE_2D,          /*!< 2D GPS */
    GPS_MODE_3D           /*!< 3D GPS */
} gps_fix_mode_t;

/**
 * @brief GPS satellite information
 *
 */
typedef struct {
    uint8_t num;       /*!< Satellite number */
    uint8_t elevation; /*!< Satellite elevation */
    uint16_t azimuth;  /*!< Satellite azimuth */
    uint8_t snr;       /*!< Satellite signal noise ratio */
} gps_satellite_t;

/**
 * @brief GPS time
 *
 */
typedef struct {
    uint8_t hour;      /*!< Hour */
    uint8_t minute;    /*!< Minute */
    uint8_t second;    /*!< Second */
    uint16_t thousand; /*!< Thousand */
} gps_time_t;

/**
 * @brief GPS date
 *
 */
typedef struct {
    uint8_t day;   /*!< Day (start from 1) */
    uint8_t month; /*!< Month (start from 1) */
    uint16_t year; /*!< Year (start from 2000) */
} gps_date_t;

/**
 * @brief NMEA Statement
 *
 */
typedef enum {
    STATEMENT_UNKNOWN = 0, /*!< Unknown statement */
    STATEMENT_GGA,         /*!< GGA */
    STATEMENT_GSA,         /*!< GSA */
    STATEMENT_RMC,         /*!< RMC */
    STATEMENT_GSV,         /*!< GSV */
    STATEMENT_GLL,         /*!< GLL */
    STATEMENT_VTG          /*!< VTG */
} nmea_statement_t;

/**
 * @brief GPS object
 *
 */
typedef struct {
    float latitude;                                                /*!< Latitude (degrees) */
    float longitude;                                               /*!< Longitude (degrees) */
    float altitude;                                                /*!< Altitude (meters) */
    gps_fix_t fix;                                                 /*!< Fix status */
    uint8_t sats_in_use;                                           /*!< Number of satellites in use */
    gps_time_t tim;                                                /*!< time in UTC */
    gps_fix_mode_t fix_mode;                                       /*!< Fix mode */
    uint8_t sats_id_in_use[GPS_MAX_SATELLITES_IN_USE];             /*!< ID list of satellite in use */
    float dop_h;                                                   /*!< Horizontal dilution of precision */
    float dop_p;                                                   /*!< Position dilution of precision  */
    float dop_v;                                                   /*!< Vertical dilution of precision  */
    uint8_t sats_in_view;                                          /*!< Number of satellites in view */
    gps_satellite_t sats_desc_in_view[GPS_MAX_SATELLITES_IN_VIEW]; /*!< Information of satellites in view */
    gps_date_t date;                                               /*!< Fix date */
    bool valid;                                                    /*!< GPS validity */
    float speed;                                                   /*!< Ground speed, unit: m/s */
    float cog;                                                     /*!< Course over ground */
    float variation;                                               /*!< Magnetic variation */
} gps_t;

/**
 * @brief GPS parser library runtime structure
 *
 */
typedef struct {
    uint8_t item_pos;                              /*!< Current position in item */
    uint8_t item_num;                              /*!< Current item number */
    uint8_t asterisk;                              /*!< Asterisk detected flag */
    uint8_t crc;                                   /*!< Calculated CRC value */
    uint8_t parsed_statement;                      /*!< OR'd of statements that have been parsed */
    uint8_t sat_num;                               /*!< Satellite number */
    uint8_t sat_count;                             /*!< Satellite count */
    uint8_t cur_statement;                         /*!< Current statement ID */
    uint32_t all_statements;                       /*!< All statements mask */
    char item_str[NMEA_MAX_STATEMENT_ITEM_LENGTH]; /*!< Current item */
    gps_t parent;                                  /*!< Parent class */
    uint8_t *buffer;                               /*!< Runtime buffer */
} esp_gps_t;

/**
 * @brief NMEA Parser Handle
 *
 */
typedef esp_gps_t *nmea_parser_handle_t;

/**
 * @brief Init NMEA Parser
 *
 * @return nmea_parser_handle_t handle of NMEA parser
 */
nmea_parser_handle_t nmea_parser_create();

/**
 * @brief Deinit NMEA Parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t nmea_parser_free(nmea_parser_handle_t nmea_hdl);

/**
 * @brief Parse NMEA statements from GPS receiver
 *
 * @param esp_gps esp_gps_t type object
 * @param len number of bytes to decode
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t nmea_parser_decode(nmea_parser_handle_t esp_gps);

#ifdef __cplusplus
}
#endif
