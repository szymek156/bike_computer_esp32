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

/** @brief porting Adafruit library for Sharp memory LCD's to the ESP32
 *  https://github.com/adafruit/Adafruit_SHARP_Memory_Display
 */
#pragma once
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include <paint.h>

namespace bk {
class Driver {
 public:
    Driver(uint16_t width = 400, uint16_t height = 256);
    void init();
    void clearDisplay();
    void refresh(void);

 private:
    static constexpr const char *TAG = "MEM_LCD";

    uint16_t width_;
    uint16_t height_;
    uint8_t vcom_;
    uint8_t *buffer_;

    spi_device_handle_t spi_;

    Paint paint_;
    void clearDisplayBuffer();
    void toggleVCom();

    void digitalWrite(gpio_num_t pin, int value);
    int digitalRead(gpio_num_t pin);
    void transfer(uint8_t *data, size_t len);
};

}  // namespace bk
