#pragma once
#include "abstract_task.h"
#include "freertos/FreeRTOS.h"
#include "nmea_parser.h"
#include "sensor_data.h"
#include "uart_wrapper.h"

namespace bk {
class GNSS : public AbstractTask {
 public:
    GNSS();
    ~GNSS();

    void start() override;

 protected:
    void run() override;

    /** @brief Sets correct datetime timezone according to the position */
    void setCorrectTZ(GNSSData &data);

    static constexpr const char *TAG = "GNSS";
    QueueHandle_t event_queue_;
    UART uart_;
    // TODO: This code is shit. Remove nmea parsing and use UBX protocol
    nmea_parser_handle_t nmea_;
};
}  // namespace bk