#pragma once
#include "abstract_task.h"
#include "event_dispatcher.h"
#include "freertos/timers.h"
#include "listerers_interface.h"
#include "sensor_data.h"

namespace bk {
class TimeService : public AbstractTask, public GNSSListener {
 public:
    TimeService();
    virtual ~TimeService();

    void onTimerExpire();

    void setEventDispatcher(IEventDispatcher *events);

    virtual void start() override;
    virtual void run() override;

    virtual void onGNSSData(const GNSSData &data) override;

 private:
    static constexpr const char *TAG = "TimeService";
    static const int GNSS_RELOAD = 10;
    
    TimerHandle_t timer_;
    IEventDispatcher *events_;

    int gnss_counter_;
};
}  // namespace bk
