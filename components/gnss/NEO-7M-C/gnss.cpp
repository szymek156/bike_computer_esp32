#include "gnss.h"

#include "sensor_data.h"
// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>
#include <string.h>
#include <uart_wrapper.h>

namespace bk {
static const int BAUD_RATE = 9600;
static const int RX_PIN = 16;
static const int UART_PORT = UART_NUM_2;

GNSS::GNSS()
    : AbstractTask(sizeof(GNSSData)),
      event_queue_(nullptr),
      uart_(BAUD_RATE, RX_PIN, UART_PORT, &event_queue_),
      nmea_(nmea_parser_create()) {
}

GNSS::~GNSS() {
    nmea_parser_free(nmea_);
}

void GNSS::start() {
    start_execution("GNSS");
}

void GNSS::run() {
    uart_event_t event;
    while (true) {
        if (xQueueReceive(event_queue_, &event, pdMS_TO_TICKS(200))) {
            switch (event.type) {
                case UART_DATA:
                    break;
                case UART_FIFO_OVF:
                    ESP_LOGW(TAG, "HW FIFO Overflow");
                    uart_.flush();
                    xQueueReset(event_queue_);
                    break;
                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "Ring Buffer Full");
                    uart_.flush();
                    xQueueReset(event_queue_);
                    break;
                case UART_BREAK:
                    ESP_LOGW(TAG, "Rx Break");
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGE(TAG, "Parity Error");
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGE(TAG, "Frame Error");
                    break;
                case UART_PATTERN_DET:
                    if (uart_.readLine(nmea_->buffer)) {
                        ESP_LOGV(TAG, "pattern: %s", nmea_->buffer);

                        if (nmea_parser_decode(nmea_) == ESP_OK) {
                            if (strstr((const char *)nmea_->buffer, "$GPGLL") != NULL) {
                                // Reduce spamming only to GPGLL sentence
                                gps_t &gps = nmea_->parent;
                                /* print information parsed from GPS statements */
                                ESP_LOGD(TAG,
                                         "%d/%d/%d %d:%d:%d => \r\n"
                                         "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
                                         "\t\t\t\t\t\tlongitude = %.05f°E\r\n"
                                         "\t\t\t\t\t\taltitude   = %.02fm\r\n"
                                         "\t\t\t\t\t\tspeed      = %fm/s\r\n"
                                         "\t\t\t\t\t\tfix   = %d\r\n"
                                         "\t\t\t\t\t\tsats in use   = %d\r\n"
                                         "\t\t\t\t\t\tsats in view   = %d\r\n"
                                         "\t\t\t\t\t\tvalid   = %d\r\n",
                                         gps.date.year,
                                         gps.date.month,
                                         gps.date.day,
                                         gps.tim.hour,
                                         gps.tim.minute,
                                         gps.tim.second,
                                         gps.latitude,
                                         gps.longitude,
                                         gps.altitude,
                                         gps.speed,
                                         gps.fix_mode,
                                         gps.sats_in_use,
                                         gps.sats_in_view,
                                         gps.valid);

                                GNSSData data;
                                data.date.day = gps.date.day;
                                data.date.month = gps.date.month;
                                data.date.year = gps.date.year + 2000;

                                data.time.hour = gps.tim.hour;
                                data.time.min = gps.tim.minute;
                                data.time.sec = gps.tim.second;

                                data.latitude = gps.latitude;
                                data.longitude = gps.longitude;
                                data.altitude = gps.altitude;

                                // TODO: to km/h
                                data.speed_kmh = gps.speed * 3.6;
                                data.track_degrees = gps.cog;

                                data.fix_status = (GNSSData::FixStatus)gps.fix_mode;

                                // TODO:

                                //    float hdop = minmea_tofloat(&frame.hdop);

                                //     using qual = messages::GPSData::FixQuality;
                                //     static const qual qualities[] =
                                //     { qual::ideal, qual::excellent, qual::good, qual::moderate,
                                //             qual::fair, qual::poor};

                                //     _gpsData.fixQuality = *std::find_if(std::cbegin(qualities),
                                //     std::cend(qualities),
                                //                             [hdop](qual range)
                                //                             { return hdop <= range || range ==
                                //                             qual::poor;});

                                // data.fix_quality
                                data.sats_in_view = gps.sats_in_view;
                                data.sats_tracked = gps.sats_in_use;

                                if (xQueueOverwrite(queue_, &data) != pdPASS) {
                                    ESP_LOGE(TAG, "Failed to send data");
                                }
                            }
                        }
                    } else {
                        ESP_LOGW(TAG, "Failed to read line from UART");
                    }
                    // esp_handle_uart_pattern(esp_gps);
                    break;
                default:
                    ESP_LOGW(TAG, "unknown uart event type: %d", event.type);
                    break;
            }
        }
    }
}
}  // namespace bk