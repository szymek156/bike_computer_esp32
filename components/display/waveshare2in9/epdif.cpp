/**
 *  @filename   :   epdif.cpp
 *  @brief      :   Implements EPD interface functions
 *                  Users have to implement all the functions in epdif.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "epdif.h"

#include "freertos/task.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

static spi_device_handle_t spi_;

EpdIf::EpdIf() {
}
EpdIf::~EpdIf() {
}

void EpdIf::DigitalWrite(gpio_num_t pin, int value) {
    // ESP_LOGD(TAG, "Set Pin %i: %i", pin, value);
    gpio_set_level(pin, value);
}

int EpdIf::DigitalRead(gpio_num_t pin) {
    int level = gpio_get_level(pin);
    // ESP_LOGD(TAG, "Get Pin %i: %i", pin, level);
    return level;
}

void EpdIf::DelayMs(unsigned int delaytime) {
    vTaskDelay(pdMS_TO_TICKS(delaytime));
}

void EpdIf::SpiTransfer(unsigned char data) {
    spi_transaction_t t = {};
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8;  // transaction length is in bits
    t.tx_data[0] = data;
    t.tx_data[1] = data;
    t.tx_data[2] = data;
    t.tx_data[3] = data;

    // Transmit!
    ESP_ERROR_CHECK(spi_device_transmit(spi_, &t));  // Should have had no issues.
}

int EpdIf::IfInit(void) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((uint64_t)1 << (uint64_t)DC_PIN) | ((uint64_t)1 << (uint64_t)RST_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_config_t i_conf = {};
    i_conf.intr_type = GPIO_INTR_DISABLE;
    i_conf.mode = GPIO_MODE_INPUT;
    i_conf.pin_bit_mask = ((uint64_t)1 << (uint64_t)BUSY_PIN);
    i_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    i_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&i_conf));

    // gpio_set_direction(DC_PIN, GPIO_MODE_OUTPUT);
    // gpio_set_direction(RST_PIN, GPIO_MODE_OUTPUT);
    // gpio_set_direction(BUSY_PIN, GPIO_MODE_INPUT);

    spi_bus_config_t buscfg = {};
    buscfg.miso_io_num = -1;
    buscfg.mosi_io_num = MOSI_PIN;
    buscfg.sclk_io_num = CLK_PIN;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 0));

    spi_device_interface_config_t devcfg = {};
    devcfg.command_bits = 0;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.mode = 0;
    devcfg.clock_speed_hz = SPI_MASTER_FREQ_8M;
    devcfg.spics_io_num = CS_PIN;
    devcfg.queue_size = 1;

    // Attach the EPD to the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi_));

    return 0;
}
