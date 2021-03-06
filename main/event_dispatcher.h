#pragma once

#include "abstract_task.h"
#include "freertos/queue.h"
#include "listerers_interface.h"
#include "sensor_data.h"

#include <set>

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
// void pollQueues() ;
//     auto total_queue_size = bk::Weather::QUEUE_SIZE + bk::GNSS::QUEUE_SIZE;

//     QueueSetHandle_t poll = xQueueCreateSet(total_queue_size);

//     configASSERT(xQueueAddToSet(weather.getQueue(), poll) == pdPASS);
//     configASSERT(xQueueAddToSet(gnss.getQueue(), poll) == pdPASS);

//     ESP_LOGI(TAG, "Polling the queue set");
//     while (true) ;
//         ESP_LOGI(TAG, "Select...");
//         QueueSetMemberHandle_t queue = xQueueSelectFromSet(poll, portMAX_DELAY);
//         // QueueSetMemberHandle_t queue = nullptr;

//         while ((queue = xQueueSelectFromSet(poll, pdMS_TO_TICKS(100))) != nullptr) {
//             if (queue == weather.getQueue()) {
//                   ESP_LOGV(TAG, "Got data from Weather");
//                   bk::WeatherData data;
//                   configASSERT(xQueueReceive(queue, &data, 0) == pdPASS);

//                   display.drawWeatherData(data);
//                   // display.invalidate();
//             } else if (queue == gnss.getQueue()) {
//                   ESP_LOGV(TAG, "Got data from GNSS");
//                   bk::GNSSData data;
//                   configASSERT(xQueueReceive(queue, &data, 0) == pdPASS);

//                   display.drawGNSSData(data);
//                   // display.invalidate();
//             } else {
//                   ESP_LOGV(TAG, "else");
//                   configASSERT(false);
//             }
//         }

//       //   ESP_LOGV(TAG, "leave while");

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
    virtual void subForWidgetChange(WidgetListener *listener) = 0;
    virtual void subForActivityData(ActivityDataListener *listener) = 0;
    virtual void subForBLEEvents(BluetoothEventListener *listener) = 0;

    virtual void unSubForKeypad(KeypadListener *listener) = 0;
    virtual void unSubForGNSS(GNSSListener *listener) = 0;
    virtual void unSubForWeather(WeatherListener *listener) = 0;
    virtual void unSubForTime(TimeListener *listener) = 0;
    virtual void unSubForWidgetChange(WidgetListener *listener) = 0;
    virtual void unSubForActivityData(ActivityDataListener *listener) = 0;
    virtual void unSubForBLEEvents(BluetoothEventListener *listener) = 0;

    virtual void widgetEvent(const WidgetData &data) = 0;
    virtual void activityDataEvent(const ActivityData &data) = 0;
};

class EventDispatcher : public IEventDispatcher {
 public:
    EventDispatcher(QueueHandle_t weather,
                    QueueHandle_t gnss,
                    QueueHandle_t keypad,
                    QueueHandle_t time,
                    QueueHandle_t bluetooth);

    virtual void listenForEvents() override;

    virtual void subForKeypad(KeypadListener *listener) override;

    virtual void subForGNSS(GNSSListener *listener) override;

    virtual void subForWeather(WeatherListener *listener) override;

    virtual void subForTime(TimeListener *listener) override;

    virtual void subForWidgetChange(WidgetListener *listener) override;

    virtual void subForActivityData(ActivityDataListener *listener) override;

    virtual void subForBLEEvents(BluetoothEventListener *listener) override;

    virtual void unSubForKeypad(KeypadListener *listener) override;

    virtual void unSubForGNSS(GNSSListener *listener) override;

    virtual void unSubForWeather(WeatherListener *listener) override;

    virtual void unSubForTime(TimeListener *listener) override;

    virtual void unSubForWidgetChange(WidgetListener *listener) override;

    virtual void unSubForActivityData(ActivityDataListener *listener) override;

    virtual void unSubForBLEEvents(BluetoothEventListener *listener) override;

    void widgetEvent(const WidgetData &data) override;

    void activityDataEvent(const ActivityData &data) override;

 private:
    static constexpr const char *TAG = "EventDispatcher";

    QueueHandle_t weather_q_;
    QueueHandle_t gnss_q_;
    QueueHandle_t keypad_q_;
    QueueHandle_t time_q_;
    QueueHandle_t bluetooth_q_;

    // TODO: that could be separate event service, but what the hell?
    QueueHandle_t widget_q_;
    QueueHandle_t activity_q_;
    QueueHandle_t health_q_;

    std::set<KeypadListener *>::iterator keypad_iter_;
    std::set<KeypadListener *> keypad_listeners_;
    std::set<GNSSListener *> gnss_listeners_;
    std::set<WeatherListener *> weather_listeners_;
    std::set<TimeListener *> time_listeners_;
    std::set<WidgetListener *> widget_listeners_;
    std::set<ActivityDataListener *> activity_listeners_;
    std::set<BluetoothEventListener *> bluetooth_listeners_;

    void notifyKeypad(const KeypadData &data);

    void notifyGNSS(const GNSSData &data);

    void notifyWeather(const WeatherData &data);

    void notifyTime(const TimeData &data);

    void notifyWidgetChange(const WidgetData &data);

    void notifyActivityData(const ActivityData &data);

    void notifyBluetoothEvent(const BLEStatusData &data);
};

}  // namespace bk