#include "time_service.hpp"

#include "sensor_data.h"

#include <ctime>

#include <sys/time.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {
void TimerCallback(TimerHandle_t timer) {
    // Naming is unfortunate, we need to live with it
    auto time_service = (TimeService *)pvTimerGetTimerID(timer);

    time_service->onTimerExpire();
}

TimeService::TimeService()
    : AbstractTask(sizeof(TimeData)),
      timer_(xTimerCreate(TAG, pdMS_TO_TICKS(1000), true, this, TimerCallback)),
      events_(nullptr),
      gnss_counter_(0) {
}

TimeService::~TimeService() {
    // TODO: This is asking for a segfault, fix later
    xTimerStop(timer_, 0);
    xTimerDelete(timer_, 0);
    events_->unSubForGNSS(this);
}

void TimeService::start() {
    events_->subForGNSS(this);
    xTimerStart(timer_, 0);
}

void TimeService::run() {
}

void TimeService::setEventDispatcher(IEventDispatcher *events) {
    events_ = events;
}

void TimeService::onGNSSData(const GNSSData &data) {
    // TODO: use something like https://github.com/BertoldVdb/ZoneDetect
    // https://www.iana.org/time-zones
    // TODO: TZ could be set to the system using esp32 api
    // TODO: chrono calendar would be used someday

    // This is cumbersome, but handling date and time in shitty in C++
    // and even shittier (congratulations!) in C/POSIX whatever you call it

    // Some meaningful data
    if (data.date_time.tm_year + 1900 > 2020) {
        if (gnss_counter_ == 0) {
            gnss_counter_ = GNSS_RELOAD;

            auto dt_copy = data.date_time;
            // UTC
            std::time_t secs_from_epoch = mktime(&(dt_copy));

            timeval t_val = {.tv_sec = secs_from_epoch, .tv_usec = 0};

            settimeofday(&t_val, nullptr);

            setenv("TZ", "CET2CEST0", 1);
            tzset();

            time_t now;
            char strftime_buf[128];
            struct tm timeinfo;

            time(&now);

            localtime_r(&now, &timeinfo);
            strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
            ESP_LOGI(TAG, "Time sync: %s", strftime_buf);
        }

        gnss_counter_--;
    }
}

void TimeService::onTimerExpire() {
    TimeData data = {};
    if (xQueueOverwrite(queue_, &data) != pdPASS) {
        ESP_LOGE(TAG, "Failed to send data");
    }
}
}  // namespace bk
