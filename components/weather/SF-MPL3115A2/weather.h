#pragma once
#include "abstract_task.h"

#include <i2c_wrapper.h>

// Arduino library to peek up the solution:
// https://learn.sparkfun.com/tutorials/mpl3115a2-pressure-sensor-hookup-guide
namespace bk {
class Weather : public AbstractTask {
 public:
    Weather();
    virtual ~Weather() = default;
    void start() override;

 protected:
    static constexpr const char* TAG = "Weather";
    void run() override;
    I2C i2c_;
};
}  // namespace bk
