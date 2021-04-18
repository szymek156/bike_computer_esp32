#include "application.h"

#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "gnss.h"
#include "keypad.h"
#include "listerers_interface.h"
#include "sensor_data.h"
#include "status_presenter.h"
#include "time_service.hpp"
#include "weather.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

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

void setupGPIO() {
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_SEL_21 | GPIO_SEL_22 | GPIO_SEL_23 | GPIO_SEL_34;

    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&io_conf);
}

namespace bk {

// TODO: Idea
// EventDispatcher is an event broker, and publisher of them
// Pages will sub and unsub for events, each one selects those which are needed - similar way as Java listeners
// Pages have onEnter onLeave methods meaning gained, and lost focus, where they can decide on 
// which events to start, or stop listen to
//
// with EventDispatcher it is possible to notify many observers, even these, which are not currently displaying data
// That may become crucial in the future
//
// For example
// Status presenter will sub for time event and gnss to have periodic updates of date and gnss status
// TimeService shall listen for GNSS and upon a fix sync system time

class EventDispatcher {
 public:
    EventDispatcher(AbstractTask *weather,
                    AbstractTask *gnss,
                    AbstractTask *keypad,
                    AbstractTask *time)
        : weather_(weather),
          gnss_(gnss),
          keypad_(keypad),
          time_(time) {
        weather_->start();
        gnss_->start();
        keypad_->start();
        time_->start();
    }

    void listenForEvents() {
        auto *weather_q = weather_->getQueue();
        auto *gnss_q = gnss_->getQueue();
        auto *keypad_q = keypad_->getQueue();

        WeatherData weather_data = {};
        GNSSData gnss_data = {};
        KeypadData keypad_data = {};

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

    // TODO:
    void subForKeypad(KeypadListener *listener) {
    }
    void subForGNSS(GNSSListener *listener) {
    }
    void subForWeather(WeatherListener *listener) {
    }
    void subForTime(TimeListener *listener) {
    }

    void unSubForKeypad(KeypadListener *listener) {
    }
    void unSubForGNSS(GNSSListener *listener) {
    }
    void unSubForWeather(WeatherListener *listener) {
    }
    void unSubForTime(TimeListener *listener) {
    }

 private:
    static constexpr const char *TAG = "EventDispatcher";

    AbstractTask *weather_;
    AbstractTask *gnss_;
    AbstractTask *keypad_;
    AbstractTask *time_;

    std::vector<KeypadListener *> keypad_listeners_;
    std::vector<GNSSListener *> gnss_listeners_;
    std::vector<WeatherListener *> weather_listeners_;
    std::vector<TimeListener *> time_listeners_;

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

void StartApplication() {
    static const char *TAG = "Broker";

    setupGPIO();

    bk::Weather weather;
    bk::GNSS gnss;
    bk::Display display;
    bk::Keypad keypad;
    bk::TimeService time_service;

    bk::EventDispatcher dispatcher(&weather, &gnss, &keypad, &time_service);

    bk::StatusPresenter status(&display);

    dispatcher.listenForEvents();
}