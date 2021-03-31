#pragma once
#include "abstract_task.h"
#include "gpio_wrapper.h"
#include "freertos/semphr.h"
#include <cstdint>

namespace bk {

class Keypad : public AbstractTask {
 public:
    Keypad();
    ~Keypad() = default;

    void start() override;

 protected:
    static constexpr const char *TAG = "Keypad";
    void run() override;

    static void handleLU(void *arg);
    static void handleRU(void *arg);
    static void handleLD(void *arg);
    static void handleRD(void *arg);

    void buttonPressed(int gpio_pin);

    bool lu_pressed_;
    bool ru_pressed_;
    bool ld_pressed_;
    bool rd_pressed_;

    GPIOWrapper gpio_;
    SemaphoreHandle_t button_pressed_;
};

}  // namespace bk