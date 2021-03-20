#include "uart_wrapper.h"

#include <esp_log.h>

namespace bk {
static const size_t BUFFER_SIZE = 512;

UART::UART(int baudrate, int rx_pin, int uart_port, QueueHandle_t* event_queue)
    : uart_port_(uart_port) {
    uart_config_t uart_config = {
        .baud_rate = baudrate,
        // Following changes rarely, hence set to constant values
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    static const int EVENT_QUEUE_SIZE = 16;

    ESP_ERROR_CHECK(uart_driver_install(
        uart_port_, 1024 /* ring buffer size */, 0, EVENT_QUEUE_SIZE, event_queue, 0));

    ESP_ERROR_CHECK(uart_param_config(uart_port_, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(
        uart_port_, UART_PIN_NO_CHANGE, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Set pattern interrupt, used to detect the end of a line */
    ESP_ERROR_CHECK(uart_enable_pattern_det_baud_intr(uart_port_, '\n', 1, 9, 0, 0));

    /* Set pattern queue size */
    ESP_ERROR_CHECK(uart_pattern_queue_reset(uart_port_, EVENT_QUEUE_SIZE));

    ESP_ERROR_CHECK(uart_flush(uart_port_));
}

UART::~UART() {
    uart_driver_delete(uart_port_);
}

void UART::flush() {
    uart_flush(uart_port_);
}

bool UART::readLine(uint8_t* buffer) {
    int pos = uart_pattern_pop_pos(uart_port_);
    if (pos != -1) {
        /* read one line(include '\n') */
        int read_len = uart_read_bytes(uart_port_, buffer, pos + 1, 100 / portTICK_PERIOD_MS);
        /* make sure the line is a standard string */
        buffer[read_len] = '\0';
        return true;
    } else {
        ESP_LOGW(TAG, "Pattern Queue Size too small");
        uart_flush_input(uart_port_);
        return false;
    }
}

}  // namespace bk