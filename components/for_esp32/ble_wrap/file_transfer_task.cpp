#include "file_transfer_task.h"

#include <cstring>
#include <fstream>

#include <esp_gatts_api.h>
#include <esp_log.h>

namespace bk {
static char buffer[ESP_GATT_MAX_ATTR_LEN] = {};

FileTransferTask::FileTransferTask(const FileInfo& file_info, const AllThatBLECrap& ble_info)
    : AbstractTask(sizeof(FileTransferData), QUEUE_SIZE),
      data_{},
      file_info_{file_info},
      ble_info_(ble_info),
      state_(TransferState::NotStarted) {
}

void FileTransferTask::start() {
    ESP_LOGI(TAG, "Start");

    start_execution(TAG);
}

void FileTransferTask::run() {
    ESP_LOGI(TAG, "Run");

    std::string filepath = "/storage/" + file_info_.filename;
    std::ifstream file(filepath, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        ESP_LOGE(TAG, "Unable to open file %s", filepath.c_str());
        state_ = TransferState::Failed;
        // Returning from the task panics the OS
        // This is insane, but valid, it's possible to
        // delete a task by themself while running.
        vTaskDelete(getTask());
    }

    int total_read = 0;

    state_ = TransferState::Ongoing;

    while (true) {
        auto left = file_info_.size - total_read;

        if (left == 0) {
            ESP_LOGI(TAG, "Upload of %s complete", filepath.c_str());
            state_ = TransferState::Finished;

            vTaskDelete(getTask());
        }

        auto to_read = left < ble_info_.max_payload ? left : ble_info_.max_payload;

        total_read += to_read;

        file.read(buffer, to_read);

        ESP_LOGI(TAG,
                 "Sent %u out of %ld, %.2f%%",
                 total_read,
                 file_info_.size,
                 total_read / (float)file_info_.size);

        bool needs_confirmation = true;

        esp_ble_gatts_send_indicate(ble_info_.gatts_if,
                                    ble_info_.app_id,
                                    ble_info_.characteristic_handle,
                                    to_read,
                                    (uint8_t*)buffer,
                                    needs_confirmation);

        // TODO: there should be some timeout, after it - delete
        // state_ = TransferState::Failed;
        ulTaskNotifyTake(true, portMAX_DELAY);
    }
}

}  // namespace bk
