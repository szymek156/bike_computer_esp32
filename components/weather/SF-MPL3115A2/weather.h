#pragma once

#include <i2c_wrapper.h>
// Arduino library to peek up the solution:
// https://learn.sparkfun.com/tutorials/mpl3115a2-pressure-sensor-hookup-guide
namespace bk {
class Weather {
 public:
    Weather();
    virtual ~Weather();
    void run();

    static constexpr const char* TAG = "Weather";

 protected:
    bool init();
    I2C i2c_;
};
}  // namespace bk
