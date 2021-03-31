#pragma once
#include "sensor_data.h"

#include <cstdint>

#include <driver/gpio.h>
#include <esp_err.h>

namespace bk {
static const int LU = 21;
static const int RU = 34;
static const int LD = 22;
static const int RD = 23;

class GPIOWrapper {
 public:
    GPIOWrapper();
    virtual ~GPIOWrapper();

    int getLevel(int gpio_pin);

    esp_err_t setLevel(int gpio_pin, int level);
    esp_err_t registerInterruptHandler(int gpio_pin, gpio_isr_t handler, void *arg);

 protected:
    static constexpr const char *TAG = "GPIOWrapper";
    static void handleInterrupt(void *arg);
};

}  // namespace bk