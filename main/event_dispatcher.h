#pragma once

#include "abstract_task.h"
#include "freertos/queue.h"
#include "listerers_interface.h"
#include "sensor_data.h"

#include <set>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

namespace bk {

// TODO: Idea
// EventDispatcher is an event broker, and publisher of them
// Pages will sub and unsub for events, each one selects those which are needed - similar way as
// Java listeners Pages have onEnter onLeave methods meaning gained, and lost focus, where they can
// decide on which events to start, or stop listen to
//
// with EventDispatcher it is possible to notify many observers, even these, which are not currently
// displaying data That may become crucial in the future
//
// For example
// Status presenter will sub for time event and gnss to have periodic updates of date and gnss
// status TimeService shall listen for GNSS and upon a fix sync system time

/** @brief Code is unused due to a bug in freertos/queue.c implementation.
 * QueueSet is not working for queues which do have "overwrite" policy.
 * FreeRTOS Version 10.3 has a fix. ESP32 uses for now 10.2
 */
// void pollQueues() {
//     auto total_queue_size = bk::Weather::QUEUE_SIZE + bk::GNSS::QUEUE_SIZE;

//     QueueSetHandle_t poll = xQueueCreateSet(total_queue_size);

//     configASSERT(xQueueAddToSet(weather.getQueue(), poll) == pdPASS);
//     configASSERT(xQueueAddToSet(gnss.getQueue(), poll) == pdPASS);

//     ESP_LOGI(TAG, "Polling the queue set");
//     while (true) {
//         ESP_LOGI(TAG, "Select...");
//         QueueSetMemberHandle_t queue = xQueueSelectFromSet(poll, portMAX_DELAY);
//         // QueueSetMemberHandle_t queue = nullptr;

//         // while ((queue = xQueueSelectFromSet(poll, pdMS_TO_TICKS(100))) != nullptr) {
//         if (queue == weather.getQueue()) {
//             ESP_LOGV(TAG, "Got data from Weather");
//             bk::WeatherData data;
//             configASSERT(xQueueReceive(queue, &data, 0) == pdPASS);

//             display.drawWeatherData(data);
//             // display.invalidate();
//         } else if (queue == gnss.getQueue()) {
//             ESP_LOGV(TAG, "Got data from GNSS");
//             bk::GNSSData data;
//             configASSERT(xQueueReceive(queue, &data, 0) == pdPASS);

//             display.drawGNSSData(data);
//             // display.invalidate();
//         } else {
//             ESP_LOGV(TAG, "else");
//             configASSERT(false);
//         }
//         // }

//         // ESP_LOGV(TAG, "leave while");

//         ESP_LOGV(TAG, "invalidate");
//         display.invalidate();
//     }
// }

class IEventDispatcher {
 public:
    virtual ~IEventDispatcher() = default;

    virtual void listenForEvents() = 0;

    virtual void subForKeypad(KeypadListener *listener) = 0;
    virtual void subForGNSS(GNSSListener *listener) = 0;
    virtual void subForWeather(WeatherListener *listener) = 0;
    virtual void subForTime(TimeListener *listener) = 0;

    virtual void unSubForKeypad(KeypadListener *listener) = 0;
    virtual void unSubForGNSS(GNSSListener *listener) = 0;
    virtual void unSubForWeather(WeatherListener *listener) = 0;
    virtual void unSubForTime(TimeListener *listener) = 0;
};

class EventDispatcher : public IEventDispatcher {
 public:
    EventDispatcher(AbstractTask *weather,
                    AbstractTask *gnss,
                    AbstractTask *keypad,
                    AbstractTask *time)
        : weather_(weather),
          gnss_(gnss),
          keypad_(keypad),
          time_(time) {
    }

    virtual void listenForEvents() override {
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
            // bool to_invalidate = false;

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

            // if (to_invalidate) {
            //     ESP_LOGV(TAG, "Drawing new data");
            //     {
            //         std::lock_guard<std::mutex> lock(display.getBufferMutex());

            //         display.prepareCanvas();
            //         display.drawKeypadData(keypad_data);
            //         display.drawWeatherData(weather_data);
            //         // display.drawGNSSData(gnss_data);
            //         display.invalidate();
            //     }

            //     // ESP_LOGV(TAG, "invalidate");
            //     // display.draw(); // for sync call
            //     // display.invalidate(); // async call
            // }
        }
    }

    virtual void subForKeypad(KeypadListener *listener) override {
        keypad_listeners_.insert(listener);
    }

    virtual void subForGNSS(GNSSListener *listener) override {
        gnss_listeners_.insert(listener);
    }

    virtual void subForWeather(WeatherListener *listener) override {
        weather_listeners_.insert(listener);
    }

    virtual void subForTime(TimeListener *listener) override {
        time_listeners_.insert(listener);
    }

    virtual void unSubForKeypad(KeypadListener *listener) override {
        keypad_listeners_.erase(listener);
    }

    virtual void unSubForGNSS(GNSSListener *listener) override {
        gnss_listeners_.erase(listener);
    }

    virtual void unSubForWeather(WeatherListener *listener) override {
        weather_listeners_.erase(listener);
    }

    virtual void unSubForTime(TimeListener *listener) override {
        time_listeners_.erase(listener);
    }

 private:
    static constexpr const char *TAG = "EventDispatcher";

    AbstractTask *weather_;
    AbstractTask *gnss_;
    AbstractTask *keypad_;
    AbstractTask *time_;

    std::set<KeypadListener *> keypad_listeners_;
    std::set<GNSSListener *> gnss_listeners_;
    std::set<WeatherListener *> weather_listeners_;
    std::set<TimeListener *> time_listeners_;

    void notifyKeypad(const KeypadData &data) {
        for (auto *observer : keypad_listeners_) {
            observer->onButtonPressed(data);
        }
    }

    void notifyGNSS(const GNSSData &data) {
        for (auto *observer : gnss_listeners_) {
            observer->onGNSSData(data);
        }
    }

    void notifyWeather(const WeatherData &data) {
        for (auto *observer : weather_listeners_) {
            observer->onWeatherData(data);
        }
    }

    void notifyTime(const TimeData &data) {
        for (auto *observer : time_listeners_) {
            observer->onTimeData(data);
        }
    }
};

}  // namespace bk