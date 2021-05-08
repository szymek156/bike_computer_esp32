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
    // TODO: if there will be more than 1 listener on keypad, this will not work.
    // keypad_iter_ may point at anyplace in the set (it's a RB tree, not a list)
    // hence arbitrary number of listeners may be omited later at iteration.
    keypad_iter_ = keypad_listeners_.insert(listener).first;

    ESP_LOGV(
        TAG, "sub for keypad, is iterator at end? %d", keypad_iter_ == keypad_listeners_.end());
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
    keypad_iter_ = keypad_listeners_.erase(keypad_iter_);
    ESP_LOGV(
        TAG, "UN sub for keypad, is iterator at end? %d", keypad_iter_ == keypad_listeners_.end());
    // keypad_listeners_.erase(listener);
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
    for (keypad_iter_ = keypad_listeners_.begin(); keypad_iter_ != keypad_listeners_.end();
         keypad_iter_++) {
        ESP_LOGV(TAG, "!!!!!!call on button pressed");

        // OnButtonPressed will cause to swap of current widget in root window.
        // Root window calls on current widget onLeave, which causes unsub for keypad events
        // on for current widget. Erase returns an iterator, which points at .end()
        // Next step root window does, is to call onEnter for new widget.
        // This will cause sub for Keypad. insert also returns iterator pointing at added element.
        // Finally we leave onButtonPressed call.
        // iterator is incremented (again points to .end()) and leaves the loop.
        //
        // This is horrible implementation hold here just as a curiosity.
        // Works by luck, that set contains exactly one listener for keypad data, having them more
        // breaks dispatching events horribly, since insert caused by onEnter will return an iterator
        // at god knows which position in the set.
        // But also shows that is actually possible to iterate over an collection and alter it, and 
        // still not get segfault!
        (*keypad_iter_)->onButtonPressed(data);

        ESP_LOGV(TAG,
                 " left on button pressed finnaly, is iterator at the end? %d",
                 keypad_iter_ == keypad_listeners_.end());
    } // Here iterator finally increments

    // for (auto *observer : keypad_listeners_) {
    //     observer->onButtonPressed(data);
    // }
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