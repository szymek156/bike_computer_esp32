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
      time_(time),
      widget_q_(xQueueCreate(1, sizeof(WidgetData))) {
}

void EventDispatcher::EventDispatcher::listenForEvents() {
    // TODO: there should be a pool, but with current
    // esp-idf official version, FreeRTOS has a bug that
    // makes use of it impossible. However, using esp-idf
    // from master has newest RTOS with the fix
    auto *weather_q = weather_->getQueue();
    auto *gnss_q = gnss_->getQueue();
    auto *keypad_q = keypad_->getQueue();
    auto *time_q = time_->getQueue();

    WeatherData weather_data = {};
    GNSSData gnss_data = {};
    KeypadData keypad_data = {};
    TimeData time_data = {};
    WidgetData widget_data = {};

    static const TickType_t TIMEOUT = pdMS_TO_TICKS(20);

    while (true) {
        if (xQueueReceive(keypad_q, &keypad_data, TIMEOUT) == pdPASS) {
            ESP_LOGV(TAG, "Got Keypad event");
            notifyKeypad(keypad_data);
        }

        if (xQueueReceive(widget_q_, &widget_data, TIMEOUT) == pdPASS) {
            ESP_LOGV(TAG, "Got Widget event");
            notifyWidgetChange(widget_data);
        }

        if (xQueueReceive(weather_q, &weather_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got Weather event");
            notifyWeather(weather_data);
        }

        if (xQueueReceive(gnss_q, &gnss_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got GNSS event");
            notifyGNSS(gnss_data);
        }

        if (xQueueReceive(time_q, &time_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got TimeService event");
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

void EventDispatcher::subForWidgetChange(WidgetListener *listener) {
    widget_listeners_.insert(listener);
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

void EventDispatcher::unSubForWidgetChange(WidgetListener *listener) {
    widget_listeners_.erase(listener);
}

void EventDispatcher::widgetEvent(const WidgetData &data) {
    if (xQueueOverwrite(widget_q_, &data) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send  widget data");
    }
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

void EventDispatcher::notifyWidgetChange(const WidgetData &data) {
    for (auto *observer : widget_listeners_) {
        observer->onWidgetChange(data);
    }
}
}  // namespace bk