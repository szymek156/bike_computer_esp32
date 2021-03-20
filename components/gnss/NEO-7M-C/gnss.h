#pragma once
#include "freertos/FreeRTOS.h"
#include "nmea_parser.h"
#include "uart_wrapper.h"
namespace bk {
class GNSS {
 public:
    GNSS();
    ~GNSS();

    void run();

 protected:
    static constexpr const char *TAG = "GNSS";
    QueueHandle_t event_queue_;
    UART uart_;
    // TODO: This code is shit. Remove nmea parsing and use UBX protocol
    nmea_parser_handle_t nmea_;
};
}  // namespace bk