#pragma once
#include "freertos/FreeRTOS.h"

#include <abstract_task.h>
#include <fs_wrapper.h>
#include <sensor_data.h>

namespace bk {
struct AllThatBLECrap {
    uint16_t gatts_if;
    int app_id;
    uint16_t characteristic_handle;
    int max_payload;
};

enum class TransferState {
    NotStarted,
    Ongoing,
    Finished,
    Failed
};

class FileTransferTask : public AbstractTask {
 public:
    FileTransferTask(const FileInfo &file_info, const AllThatBLECrap& ble_info);
    ~FileTransferTask() = default;

    void start() override;

    void run() override;

    TransferState get_state();

 private:
    static constexpr const char *TAG = "FileTransfer";
    static const size_t QUEUE_SIZE = 1;
    FileTransferData data_;

    FileInfo file_info_;
    AllThatBLECrap ble_info_;

    TransferState state_;
};
}  // namespace bk
