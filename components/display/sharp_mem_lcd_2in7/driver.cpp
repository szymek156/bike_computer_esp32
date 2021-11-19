/*********************************************************************
This is an Arduino library for our Monochrome SHARP Memory Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1393

These displays use SPI to communicate, 3 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include "driver.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <chrono>
#include <cstring>

#include <esp_heap_caps.h>
#include <esp_log.h>

namespace bk {
#define SHARPMEM_BIT_WRITECMD (0x01)  // 0x80 in LSB format
#define SHARPMEM_BIT_VCOM (0x02)      // 0x40 in LSB format
#define SHARPMEM_BIT_CLEAR (0x04)     // 0x20 in LSB format

// Pin definition
#define MOSI_PIN GPIO_NUM_27
#define CLK_PIN GPIO_NUM_26
#define CS_PIN GPIO_NUM_25

// Pin level definition
#define LOW 0
#define HIGH 1

#define PERF
Driver::Driver(uint16_t width, uint16_t height)
    : width_(width),
      height_(height),
      vcom_(SHARPMEM_BIT_VCOM),
      // TODO: there is no reason to have it dynamically allocated, move to global
      buffer_((uint8_t *)malloc((width_ * height_) / 8)),
      paint_(Paint(buffer_, width_, height_, Endian::Little)) {
    clearDisplayBuffer();
    paint_.SetRotate(ROTATE_0);
}

void Driver::init() {
    spi_bus_config_t buscfg = {};
    buscfg.miso_io_num = -1;
    buscfg.mosi_io_num = MOSI_PIN;
    buscfg.sclk_io_num = CLK_PIN;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    // buscfg.max_transfer_sz = 0;
    // ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_DISABLED));

    // Initialize the SPI bus
    // TODO: should we set DMA channel?
    // TODO:
    // Maximum transfer size, in bytes. Defaults to 4092 if 0 when DMA enabled,
    // or to SOC_SPI_MAXIMUM_BUFFER_SIZE if DMA is disabled.
    // #define SOC_SPI_MAXIMUM_BUFFER_SIZE 64

    size_t data_len = 1 + (1 + 1) * height_ + (height_ * width_ / 8) + 1;
    buscfg.max_transfer_sz = data_len;
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH1));

    spi_device_interface_config_t devcfg = {};
    devcfg.command_bits = 0;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.mode = 0;
    // 10MHz is the max, for higher values display shows squashed data (weird!),
    // and for even higher values (like 20MHz) it shows nothing
    // TODO: in case of troubles, reduce to 2MHz, it's a value from Arduino library
    // devcfg.clock_speed_hz = 2000000;
    // TODO: does frequency impacts battery comsumption?
    devcfg.clock_speed_hz = SPI_MASTER_FREQ_10M;

    // Keep CS high, when transmission is ongoing, set to low, when finished
    // this display is weird in that CS is active HIGH not LOW like every other
    // Data send in little-endian - another weirdo, thankfully it's easy
    // to set those up in IDF
    devcfg.flags = SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_POSITIVE_CS;
    devcfg.spics_io_num = CS_PIN;

    // TODO: decide on queue size
    devcfg.queue_size = 1;

    // Attach the EPD to the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi_));
}

void Driver::clearDisplay() {
    clearDisplayBuffer();

    uint8_t clear_data[2] = {uint8_t(vcom_ | SHARPMEM_BIT_CLEAR), 0x00};
    transfer(clear_data, 2);

    toggleVCom();
}

/** @brief each line is a single TX takes 21ms at SPI freq 10MHz */
// void Driver::refresh() {
// #if defined(PERF)
//     std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
// #endif
//     uint8_t bytes_per_line = width_ / 8;
//     uint8_t line[bytes_per_line + 4];
//     // command | address | data | trailer 2bytes
//     line[0] = uint8_t(vcom_ | SHARPMEM_BIT_WRITECMD);
//     line[bytes_per_line + 3] = 0;
//     line[bytes_per_line + 4] = 0;

//     for (auto idx = 0; idx < height_; idx++) {
//         line[1] = idx;
//         memcpy(line + 2, buffer_ + (bytes_per_line * idx), bytes_per_line);
//         transfer(line, bytes_per_line + 4);
//     }

// #if defined(PERF)
//     std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
// #endif

// #if defined(PERF)
//     ESP_LOGD(TAG,
//              "writing whole frame tx per each line %llu ms",
//              std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());
// #endif
// }

/** @brief one full TX, use DMA memory  takes 11ms at SPI freq 10MHz*/
void Driver::refresh() {
#if defined(PERF)
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
#endif
    // command + (address + trailer) * # of lines + buffer + final_trailer
    size_t data_len = 1 + (1 + 1) * height_ + (height_ * width_ / 8) + 1;

    ESP_LOGI(TAG, "Total data len %u", data_len);

    // Set all to 0, so you don't need to set trailers
    uint8_t *data = (uint8_t *)heap_caps_calloc(
        data_len, sizeof(uint8_t), MALLOC_CAP_DMA);  // calloc(data_len, sizeof(uint8_t));

    data[0] = uint8_t(vcom_ | SHARPMEM_BIT_WRITECMD);
    uint8_t *lines = data + 1;

    auto line_stride = width_ / 8;
    auto trailer = 1;
    auto address = 1;

    for (auto idx = 0; idx < height_; idx++) {
        auto row = idx * (address + line_stride + trailer);

        // address
        lines[row] = idx;
        // line data
        memcpy(lines + row + address, buffer_ + idx * line_stride, line_stride);
        // trailer
        lines[row + address + line_stride] = 0;
    }

    transfer(data, data_len);

    heap_caps_free(data);

#if defined(PERF)
    std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
#endif

#if defined(PERF)
    ESP_LOGD(TAG,
             "writing whole frame as one TX %llu ms",
             std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());
#endif
}

void Driver::clearDisplayBuffer() {
    memset(buffer_, 0xff, (width_ * height_) / 8);
}

void Driver::toggleVCom() {
    vcom_ ^= SHARPMEM_BIT_VCOM;
}

void Driver::digitalWrite(gpio_num_t pin, int value) {
    // ESP_LOGD(TAG, "Set Pin %i: %i", pin, value);
    gpio_set_level(pin, value);
}

int Driver::digitalRead(gpio_num_t pin) {
    int level = gpio_get_level(pin);
    // ESP_LOGD(TAG, "Get Pin %i: %i", pin, level);
    return level;
}

void Driver::transfer(unsigned char *data, size_t len) {
    spi_transaction_t t = {};
    // t.flags = SPI_TRANS_USE_TXDATA;
    // t.tx_data[0] = data;
    // t.tx_data[1] = data;
    // t.tx_data[2] = data;
    // t.tx_data[3] = data;

    t.length = len * 8;  // transaction length is in bits
    t.tx_buffer = data;
    // Synchronous transmit, there are async options too
    ESP_ERROR_CHECK(spi_device_transmit(spi_, &t));  // Should have had no issues.
}
}  // namespace bk
