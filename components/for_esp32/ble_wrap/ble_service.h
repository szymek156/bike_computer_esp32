#pragma once
#include "abstract_task.h"
#include "file_transfer_gatts.h"
namespace bk {
/** @brief Abstraction over bluetooth low energy */
class BLEService : public AbstractTask {
 public:
    BLEService();
    void start() override;
    void run() override;
    void enable();
    void disable();

};
}  // namespace bk