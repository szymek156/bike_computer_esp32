#pragma once
#include "abstract_task.h"
#include "file_transfer_gatts.h"
namespace bk {
/** @brief Abstraction over bluetooth low energy */
class BLEService : public AbstractTask {
 public:
    // Huh that's second singleton in the system.
    // Does it converge to total singularity?
    // Some may say that's antipattern, but I opt to have it in
    // deeply embedded device, like this one.
    // Alternativley components could use messaging, but amount
    // of boilerplate that is required to write it is not worth
    // my PRECIOUS time.
    // Apart from that, BLE API uses static entities all over the place
    // anyway, so not much loss adding another one
    static BLEService& instance();

    void start() override;
    void run() override;
    void enable();
    void disable();

private:
    BLEService();
    // TODO: operators, cpy ctors...

    bool enabled_;

};
}  // namespace bk