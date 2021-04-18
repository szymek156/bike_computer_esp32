#include "time_service.hpp"
#include "sensor_data.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {
void TimerCallback(TimerHandle_t timer) {
    // Naming is unfortunate, we need to live with it
    auto time_service = (TimeService *)pvTimerGetTimerID(timer);

    time_service->onTimerExpire();
}

TimeService::TimeService()
    : AbstractTask(sizeof(TimeData)), timer_(xTimerCreate(TAG, pdMS_TO_TICKS(1000), true, this, TimerCallback)) {
    xTimerStart(timer_, 0);
}

TimeService::~TimeService() {
    // TODO: This is asking for a segfault, fix later
    xTimerStop(timer_, 0);
    xTimerDelete(timer_, 0);
}


void TimeService::start() {

}

void TimeService::run() {

}

void TimeService::onTimerExpire() {
    // TODO: get gnss data, if fix, setup time
    // TODO: propagate tick second event

    ESP_LOGD(TAG, "TimerExpire");
}
}  // namespace bk
