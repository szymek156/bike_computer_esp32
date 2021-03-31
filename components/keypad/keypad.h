#pragma once
#include "abstract_task.h"
#include "gpio_wrapper.h"
#include "sensor_data.h"

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

    static void buttonPressed(void *arg);

    KeypadData data_;

    GPIOWrapper gpio_;
};

}  // namespace bk