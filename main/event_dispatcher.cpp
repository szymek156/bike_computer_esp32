#include "event_dispatcher.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
namespace bk {
EventDispatcher::EventDispatcher(AbstractTask *weather,
                                 AbstractTask *gnss,
                                 AbstractTask *keypad,
                                 AbstractTask *time)
    : weather_(weather),
      gnss_(gnss),
      keypad_(keypad),
      time_(time) {
}

void EventDispatcher::EventDispatcher::listenForEvents() {
    auto *weather_q = weather_->getQueue();
    auto *gnss_q = gnss_->getQueue();
    auto *keypad_q = keypad_->getQueue();
    auto *time_q = time_->getQueue();

    WeatherData weather_data = {};
    GNSSData gnss_data = {};
    KeypadData keypad_data = {};
    TimeData time_data = {};

    static const TickType_t TIMEOUT = pdMS_TO_TICKS(20);

    while (true) {
        if (xQueueReceive(keypad_q, &keypad_data, TIMEOUT) == pdPASS) {
            ESP_LOGV(TAG, "Got data from Keypad");
            notifyKeypad(keypad_data);
        }

        if (xQueueReceive(weather_q, &weather_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got data from Weather");
            notifyWeather(weather_data);
        }

        if (xQueueReceive(gnss_q, &gnss_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got data from GNSS");
            notifyGNSS(gnss_data);
        }

        if (xQueueReceive(time_q, &time_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got data from TimeService");
            notifyTime(time_data);
        }
    }
}

void EventDispatcher::subForKeypad(KeypadListener *listener) {
    keypad_listeners_.insert(listener);
}

void EventDispatcher::subForGNSS(GNSSListener *listener) {
    gnss_listeners_.insert(listener);
}

void EventDispatcher::subForWeather(WeatherListener *listener) {
    weather_listeners_.insert(listener);
}

void EventDispatcher::subForTime(TimeListener *listener) {
    time_listeners_.insert(listener);
}

void EventDispatcher::unSubForKeypad(KeypadListener *listener) {
    keypad_listeners_.erase(listener);
}

void EventDispatcher::unSubForGNSS(GNSSListener *listener) {
    gnss_listeners_.erase(listener);
}

void EventDispatcher::unSubForWeather(WeatherListener *listener) {
    weather_listeners_.erase(listener);
}

void EventDispatcher::unSubForTime(TimeListener *listener) {
    time_listeners_.erase(listener);
}
void EventDispatcher::notifyKeypad(const KeypadData &data) {
    for (auto *observer : keypad_listeners_) {
        observer->onButtonPressed(data);
    }
}

void EventDispatcher::notifyGNSS(const GNSSData &data) {
    for (auto *observer : gnss_listeners_) {
        observer->onGNSSData(data);
    }
}

void EventDispatcher::notifyWeather(const WeatherData &data) {
    for (auto *observer : weather_listeners_) {
        observer->onWeatherData(data);
    }
}

void EventDispatcher::notifyTime(const TimeData &data) {
    for (auto *observer : time_listeners_) {
        observer->onTimeData(data);
    }
}
}  // namespace bk