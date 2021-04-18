#pragma once
#include "abstract_task.h"
#include "freertos/timers.h"

namespace bk {
class TimeService : public AbstractTask {
 public:
    TimeService();
    virtual ~TimeService();
    
    void onTimerExpire();

    virtual void start() override;
    virtual void run() override;

 private:
    static constexpr const char* TAG = "TimeService";

    TimerHandle_t timer_;
};
}  // namespace bk
