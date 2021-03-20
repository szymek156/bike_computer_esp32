#pragma once
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"

namespace bk {
class UART {
 public:
    /** @brief Install UART driver with given parameters.
     * Driver sends events to the queue
     */
    UART(int baudrate, int rx_pin, int uart_port, QueueHandle_t *event_queue);
    ~UART();
    void flush();
    bool readLine(uint8_t *buffer);

 protected:
    static constexpr const char *TAG = "UART";
    int uart_port_;
};
}  // namespace bk