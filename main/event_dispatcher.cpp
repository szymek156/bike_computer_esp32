#include "event_dispatcher.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
namespace bk {
EventDispatcher::EventDispatcher(QueueHandle_t weather,
                                 QueueHandle_t gnss,
                                 QueueHandle_t keypad,
                                 QueueHandle_t time)
    : weather_q_(weather),
      gnss_q_(gnss),
      keypad_q_(keypad),
      time_q_(time),
      widget_q_(xQueueCreate(1, sizeof(WidgetData))),
      activity_q_(xQueueCreate(1, sizeof(ActivityData))) {
}

void EventDispatcher::EventDispatcher::listenForEvents() {
    // TODO: there should be a pool, but with current
    // esp-idf official version, FreeRTOS has a bug that
    // makes use of it impossible. However, using esp-idf
    // from master has newest RTOS with the fix

    WeatherData weather_data = {};
    GNSSData gnss_data = {};
    KeypadData keypad_data = {};
    TimeData time_data = {};
    WidgetData widget_data = {};
    ActivityData activity_data = {};

    static const TickType_t TIMEOUT = pdMS_TO_TICKS(20);

    while (true) {
        if (xQueueReceive(keypad_q_, &keypad_data, TIMEOUT) == pdPASS) {
            ESP_LOGV(TAG, "Got Keypad event");
            notifyKeypad(keypad_data);
        }

        if (xQueueReceive(widget_q_, &widget_data, TIMEOUT) == pdPASS) {
            ESP_LOGV(TAG, "Got Widget event");
            notifyWidgetChange(widget_data);
        }

        if (xQueueReceive(weather_q_, &weather_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got Weather event");
            notifyWeather(weather_data);
        }

        if (xQueueReceive(gnss_q_, &gnss_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got GNSS event");
            notifyGNSS(gnss_data);
        }

        if (xQueueReceive(time_q_, &time_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got TimeService event");
            notifyTime(time_data);
        }

        if (xQueueReceive(activity_q_, &activity_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got Activity event");
            notifyActivityData(activity_data);
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

void EventDispatcher::subForActivityData(ActivityDataListener *listener) {
    activity_listeners_.insert(listener);
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

void EventDispatcher::unSubForActivityData(ActivityDataListener *listener) {
    activity_listeners_.erase(listener);
}

void EventDispatcher::widgetEvent(const WidgetData &data) {
    if (xQueueOverwrite(widget_q_, &data) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send  widget data");
    }
}

void EventDispatcher::activityDataEvent(const ActivityData &data) {
    if (xQueueOverwrite(activity_q_, &data) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send  activity data");
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

void EventDispatcher::notifyActivityData(const ActivityData &data) {
    for (auto *observer : activity_listeners_) {
        observer->onActivityData(data);
    }
}
}  // namespace bk