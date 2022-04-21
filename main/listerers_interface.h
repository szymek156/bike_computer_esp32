#pragma once
#include "sensor_data.h"

namespace bk {
class GNSSListener {
 public:
    virtual void onGNSSData(const GNSSData &data) = 0;
};

class KeypadListener {
 public:
    virtual void onButtonPressed(const KeypadData &data) = 0;
};

class WeatherListener {
 public:
    virtual void onWeatherData(const WeatherData &data) = 0;
};

class TimeListener {
 public:
    virtual void onTimeData(const TimeData &data) = 0;
};

class WidgetListener {
 public:
    virtual void onWidgetChange(const WidgetData &data) = 0;
};

class ActivityDataListener {
 public:
    virtual void onActivityData(const ActivityData &data) = 0;
};

class BluetoothEventListener {
 public:
    virtual void onBluetoothChange(const BLEStatusData &data) = 0;
};

}  // namespace bk
