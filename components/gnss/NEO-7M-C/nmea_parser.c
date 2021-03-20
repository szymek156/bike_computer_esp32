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

#include "nmea_parser.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static const char *GPS_TAG = "nmea_parser";

/**
 * @brief parse latitude or longitude
 *              format of latitude in NMEA is ddmm.sss and longitude is dddmm.sss
 * @param esp_gps esp_gps_t type object
 * @return float Latitude or Longitude value (unit: degree)
 */
static float parse_lat_long(esp_gps_t *esp_gps) {
    float ll = strtof(esp_gps->item_str, NULL);
    int deg = ((int)ll) / 100;
    float min = ll - (deg * 100);
    ll = deg + min / 60.0f;
    return ll;
}

/**
 * @brief Converter two continuous numeric character into a uint8_t number
 *
 * @param digit_char numeric character
 * @return uint8_t result of converting
 */
static inline uint8_t convert_two_digit2number(const char *digit_char) {
    return 10 * (digit_char[0] - '0') + (digit_char[1] - '0');
}

/**
 * @brief Parse UTC time in GPS statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_utc_time(esp_gps_t *esp_gps) {
    esp_gps->parent.tim.hour = convert_two_digit2number(esp_gps->item_str + 0);
    esp_gps->parent.tim.minute = convert_two_digit2number(esp_gps->item_str + 2);
    esp_gps->parent.tim.second = convert_two_digit2number(esp_gps->item_str + 4);
    if (esp_gps->item_str[6] == '.') {
        uint16_t tmp = 0;
        uint8_t i = 7;
        while (esp_gps->item_str[i]) {
            tmp = 10 * tmp + esp_gps->item_str[i] - '0';
            i++;
        }
        esp_gps->parent.tim.thousand = tmp;
    }
}

/**
 * @brief Parse GGA statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_gga(esp_gps_t *esp_gps) {
    /* Process GGA statement */
    switch (esp_gps->item_num) {
        case 1: /* Process UTC time */
            parse_utc_time(esp_gps);
            break;
        case 2: /* Latitude */
            esp_gps->parent.latitude = parse_lat_long(esp_gps);
            break;
        case 3: /* Latitude north(1)/south(-1) information */
            if (esp_gps->item_str[0] == 'S' || esp_gps->item_str[0] == 's') {
                esp_gps->parent.latitude *= -1;
            }
            break;
        case 4: /* Longitude */
            esp_gps->parent.longitude = parse_lat_long(esp_gps);
            break;
        case 5: /* Longitude east(1)/west(-1) information */
            if (esp_gps->item_str[0] == 'W' || esp_gps->item_str[0] == 'w') {
                esp_gps->parent.longitude *= -1;
            }
            break;
        case 6: /* Fix status */
            esp_gps->parent.fix = (gps_fix_t)strtol(esp_gps->item_str, NULL, 10);
            break;
        case 7: /* Satellites in use */
            esp_gps->parent.sats_in_use = (uint8_t)strtol(esp_gps->item_str, NULL, 10);
            break;
        case 8: /* HDOP */
            esp_gps->parent.dop_h = strtof(esp_gps->item_str, NULL);
            break;
        case 9: /* Altitude */
            esp_gps->parent.altitude = strtof(esp_gps->item_str, NULL);
            break;
        case 11: /* Altitude above ellipsoid */
            esp_gps->parent.altitude += strtof(esp_gps->item_str, NULL);
            break;
        default:
            break;
    }
}

/**
 * @brief Parse GSA statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_gsa(esp_gps_t *esp_gps) {
    /* Process GSA statement */
    switch (esp_gps->item_num) {
        case 2: /* Process fix mode */
            esp_gps->parent.fix_mode = (gps_fix_mode_t)strtol(esp_gps->item_str, NULL, 10);
            break;
        case 15: /* Process PDOP */
            esp_gps->parent.dop_p = strtof(esp_gps->item_str, NULL);
            break;
        case 16: /* Process HDOP */
            esp_gps->parent.dop_h = strtof(esp_gps->item_str, NULL);
            break;
        case 17: /* Process VDOP */
            esp_gps->parent.dop_v = strtof(esp_gps->item_str, NULL);
            break;
        default:
            /* Parse satellite IDs */
            if (esp_gps->item_num >= 3 && esp_gps->item_num <= 14) {
                esp_gps->parent.sats_id_in_use[esp_gps->item_num - 3] =
                    (uint8_t)strtol(esp_gps->item_str, NULL, 10);
            }
            break;
    }
}

/**
 * @brief Parse GSV statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_gsv(esp_gps_t *esp_gps) {
    /* Process GSV statement */
    switch (esp_gps->item_num) {
        case 1: /* total GSV numbers */
            esp_gps->sat_count = (uint8_t)strtol(esp_gps->item_str, NULL, 10);
            break;
        case 2: /* Current GSV statement number */
            esp_gps->sat_num = (uint8_t)strtol(esp_gps->item_str, NULL, 10);
            break;
        case 3: /* Process satellites in view */
            esp_gps->parent.sats_in_view = (uint8_t)strtol(esp_gps->item_str, NULL, 10);
            break;
        default:
            if (esp_gps->item_num >= 4 && esp_gps->item_num <= 19) {
                uint8_t item_num =
                    esp_gps->item_num - 4; /* Normalize item number from 4-19 to 0-15 */
                uint8_t index;
                uint32_t value;
                index = 4 * (esp_gps->sat_num - 1) + item_num / 4; /* Get array index */
                if (index < GPS_MAX_SATELLITES_IN_VIEW) {
                    value = strtol(esp_gps->item_str, NULL, 10);
                    switch (item_num % 4) {
                        case 0:
                            esp_gps->parent.sats_desc_in_view[index].num = (uint8_t)value;
                            break;
                        case 1:
                            esp_gps->parent.sats_desc_in_view[index].elevation = (uint8_t)value;
                            break;
                        case 2:
                            esp_gps->parent.sats_desc_in_view[index].azimuth = (uint16_t)value;
                            break;
                        case 3:
                            esp_gps->parent.sats_desc_in_view[index].snr = (uint8_t)value;
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
    }
}

/**
 * @brief Parse RMC statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_rmc(esp_gps_t *esp_gps) {
    /* Process GPRMC statement */
    switch (esp_gps->item_num) {
        case 1: /* Process UTC time */
            parse_utc_time(esp_gps);
            break;
        case 2: /* Process valid status */
            esp_gps->parent.valid = (esp_gps->item_str[0] == 'A');
            break;
        case 3: /* Latitude */
            esp_gps->parent.latitude = parse_lat_long(esp_gps);
            break;
        case 4: /* Latitude north(1)/south(-1) information */
            if (esp_gps->item_str[0] == 'S' || esp_gps->item_str[0] == 's') {
                esp_gps->parent.latitude *= -1;
            }
            break;
        case 5: /* Longitude */
            esp_gps->parent.longitude = parse_lat_long(esp_gps);
            break;
        case 6: /* Longitude east(1)/west(-1) information */
            if (esp_gps->item_str[0] == 'W' || esp_gps->item_str[0] == 'w') {
                esp_gps->parent.longitude *= -1;
            }
            break;
        case 7: /* Process ground speed in unit m/s */
            esp_gps->parent.speed = strtof(esp_gps->item_str, NULL) * 1.852;
            break;
        case 8: /* Process true course over ground */
            esp_gps->parent.cog = strtof(esp_gps->item_str, NULL);
            break;
        case 9: /* Process date */
            esp_gps->parent.date.day = convert_two_digit2number(esp_gps->item_str + 0);
            esp_gps->parent.date.month = convert_two_digit2number(esp_gps->item_str + 2);
            esp_gps->parent.date.year = convert_two_digit2number(esp_gps->item_str + 4);
            break;
        case 10: /* Process magnetic variation */
            esp_gps->parent.variation = strtof(esp_gps->item_str, NULL);
            break;
        default:
            break;
    }
}

/**
 * @brief Parse GLL statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_gll(esp_gps_t *esp_gps) {
    /* Process GPGLL statement */
    switch (esp_gps->item_num) {
        case 1: /* Latitude */
            esp_gps->parent.latitude = parse_lat_long(esp_gps);
            break;
        case 2: /* Latitude north(1)/south(-1) information */
            if (esp_gps->item_str[0] == 'S' || esp_gps->item_str[0] == 's') {
                esp_gps->parent.latitude *= -1;
            }
            break;
        case 3: /* Longitude */
            esp_gps->parent.longitude = parse_lat_long(esp_gps);
            break;
        case 4: /* Longitude east(1)/west(-1) information */
            if (esp_gps->item_str[0] == 'W' || esp_gps->item_str[0] == 'w') {
                esp_gps->parent.longitude *= -1;
            }
            break;
        case 5: /* Process UTC time */
            parse_utc_time(esp_gps);
            break;
        case 6: /* Process valid status */
            esp_gps->parent.valid = (esp_gps->item_str[0] == 'A');
            break;
        default:
            break;
    }
}

/**
 * @brief Parse VTG statements
 *
 * @param esp_gps esp_gps_t type object
 */
static void parse_vtg(esp_gps_t *esp_gps) {
    /* Process GPVGT statement */
    switch (esp_gps->item_num) {
        case 1: /* Process true course over ground */
            esp_gps->parent.cog = strtof(esp_gps->item_str, NULL);
            break;
        case 3: /* Process magnetic variation */
            esp_gps->parent.variation = strtof(esp_gps->item_str, NULL);
            break;
        case 5: /* Process ground speed in unit m/s */
            esp_gps->parent.speed = strtof(esp_gps->item_str, NULL) * 1.852;  // knots to m/s
            break;
        case 7: /* Process ground speed in unit m/s */
            esp_gps->parent.speed = strtof(esp_gps->item_str, NULL) / 3.6;  // km/h to m/s
            break;
        default:
            break;
    }
}

/**
 * @brief Parse received item
 *
 * @param esp_gps esp_gps_t type object
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
static esp_err_t parse_item(esp_gps_t *esp_gps) {
    esp_err_t err = ESP_OK;
    /* start of a statement */
    if (esp_gps->item_num == 0 && esp_gps->item_str[0] == '$') {
        if (0) {
        }
        else if (strstr(esp_gps->item_str, "GGA")) {
            esp_gps->cur_statement = STATEMENT_GGA;
        }
        else if (strstr(esp_gps->item_str, "GSA")) {
            esp_gps->cur_statement = STATEMENT_GSA;
        }
        else if (strstr(esp_gps->item_str, "RMC")) {
            esp_gps->cur_statement = STATEMENT_RMC;
        }
        else if (strstr(esp_gps->item_str, "GSV")) {
            esp_gps->cur_statement = STATEMENT_GSV;
        }
        else if (strstr(esp_gps->item_str, "GLL")) {
            esp_gps->cur_statement = STATEMENT_GLL;
        }
        else if (strstr(esp_gps->item_str, "VTG")) {
            esp_gps->cur_statement = STATEMENT_VTG;
        }
        else {
            esp_gps->cur_statement = STATEMENT_UNKNOWN;
        }
        goto out;
    }
    /* Parse each item, depend on the type of the statement */
    if (esp_gps->cur_statement == STATEMENT_UNKNOWN) {
        goto out;
    }
    else if (esp_gps->cur_statement == STATEMENT_GGA) {
        parse_gga(esp_gps);
    }
    else if (esp_gps->cur_statement == STATEMENT_GSA) {
        parse_gsa(esp_gps);
    }
    else if (esp_gps->cur_statement == STATEMENT_GSV) {
        parse_gsv(esp_gps);
    }
    else if (esp_gps->cur_statement == STATEMENT_RMC) {
        parse_rmc(esp_gps);
    }
    else if (esp_gps->cur_statement == STATEMENT_GLL) {
        parse_gll(esp_gps);
    }
    else if (esp_gps->cur_statement == STATEMENT_VTG) {
        parse_vtg(esp_gps);
    }
    else {
        err = ESP_FAIL;
    }
out:
    return err;
}

esp_err_t nmea_parser_decode(nmea_parser_handle_t esp_gps) {
    const uint8_t *d = esp_gps->buffer;
    while (*d) {
        /* Start of a statement */
        if (*d == '$') {
            /* Reset runtime information */
            esp_gps->asterisk = 0;
            esp_gps->item_num = 0;
            esp_gps->item_pos = 0;
            esp_gps->cur_statement = 0;
            esp_gps->crc = 0;
            esp_gps->sat_count = 0;
            esp_gps->sat_num = 0;
            /* Add character to item */
            esp_gps->item_str[esp_gps->item_pos++] = *d;
            esp_gps->item_str[esp_gps->item_pos] = '\0';
        }
        /* Detect item separator character */
        else if (*d == ',') {
            /* Parse current item */
            parse_item(esp_gps);
            /* Add character to CRC computation */
            esp_gps->crc ^= (uint8_t)(*d);
            /* Start with next item */
            esp_gps->item_pos = 0;
            esp_gps->item_str[0] = '\0';
            esp_gps->item_num++;
        }
        /* End of CRC computation */
        else if (*d == '*') {
            /* Parse current item */
            parse_item(esp_gps);
            /* Asterisk detected */
            esp_gps->asterisk = 1;
            /* Start with next item */
            esp_gps->item_pos = 0;
            esp_gps->item_str[0] = '\0';
            esp_gps->item_num++;
        }
        /* End of statement */
        else if (*d == '\r') {
            /* Convert received CRC from string (hex) to number */
            uint8_t crc = (uint8_t)strtol(esp_gps->item_str, NULL, 16);
            /* CRC passed */
            if (esp_gps->crc == crc) {
                switch (esp_gps->cur_statement) {
                    case STATEMENT_GGA:
                        esp_gps->parsed_statement |= 1 << STATEMENT_GGA;
                        break;
                    case STATEMENT_GSA:
                        esp_gps->parsed_statement |= 1 << STATEMENT_GSA;
                        break;
                    case STATEMENT_RMC:
                        esp_gps->parsed_statement |= 1 << STATEMENT_RMC;
                        break;
                    case STATEMENT_GSV:
                        if (esp_gps->sat_num == esp_gps->sat_count) {
                            esp_gps->parsed_statement |= 1 << STATEMENT_GSV;
                        }
                        break;
                    case STATEMENT_GLL:
                        esp_gps->parsed_statement |= 1 << STATEMENT_GLL;
                        break;
                    case STATEMENT_VTG:
                        esp_gps->parsed_statement |= 1 << STATEMENT_VTG;
                        break;
                    default:
                        break;
                }
                /* Check if all statements have been parsed */
                if (((esp_gps->parsed_statement) & esp_gps->all_statements) ==
                    esp_gps->all_statements) {
                    esp_gps->parsed_statement = 0;
                }
            } else {
                ESP_LOGE(GPS_TAG, "CRC Error for statement:%s", esp_gps->buffer);
                return ESP_ERR_INVALID_CRC;
            }
            if (esp_gps->cur_statement == STATEMENT_UNKNOWN) {
                ESP_LOGE(GPS_TAG, "Unknown statement:%s", esp_gps->buffer);
                return ESP_ERR_NOT_SUPPORTED;
            }
        }
        /* Other non-space character */
        else {
            if (!(esp_gps->asterisk)) {
                /* Add to CRC */
                esp_gps->crc ^= (uint8_t)(*d);
            }
            /* Add character to item */
            esp_gps->item_str[esp_gps->item_pos++] = *d;
            esp_gps->item_str[esp_gps->item_pos] = '\0';
        }
        /* Process next character */
        d++;
    }
    return ESP_OK;
}

nmea_parser_handle_t nmea_parser_create() {
    esp_gps_t *esp_gps = calloc(1, sizeof(esp_gps_t));

    esp_gps->buffer = calloc(1, NMEA_PARSER_RUNTIME_BUFFER_SIZE);

    esp_gps->all_statements |= (1 << STATEMENT_GSA);
    esp_gps->all_statements |= (1 << STATEMENT_GSV);
    esp_gps->all_statements |= (1 << STATEMENT_GGA);
    esp_gps->all_statements |= (1 << STATEMENT_RMC);
    esp_gps->all_statements |= (1 << STATEMENT_GLL);
    esp_gps->all_statements |= (1 << STATEMENT_VTG);

    /* Set attributes */
    esp_gps->all_statements &= 0xFE;
    ESP_LOGI(GPS_TAG, "NMEA Parser init OK");
    return esp_gps;
}

esp_err_t nmea_parser_free(nmea_parser_handle_t nmea_hdl)
{
    free(nmea_hdl->buffer);
    free(nmea_hdl);
    return ESP_OK;
}