#include "health_service.h"

#include <string>

#include <fs_wrapper.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_log.h>
#include <esp_partition.h>
#include <esp_spiffs.h>
#include <stdio.h>
#include <stdlib.h>

namespace bk {
void HealthService::reportAll() {
    reportFS();
    reportRAM();
    reportCPU();
    reportOS();
}

void HealthService::reportFS() {
    ESP_LOGI(TAG, "Filesystem:");

    // TODO: in order to get the info, partition needs to be mounted
    // for (auto iter = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY,
    // nullptr);
    //      iter != nullptr;
    //      iter = esp_partition_next(iter)) {
    //     auto *partition_info = esp_partition_get(iter);

    //     size_t total = 0, used = 0;
    //     auto ret = esp_spiffs_info(partition_info->label, &total, &used);
    //     if (ret != ESP_OK) {
    //         ESP_LOGE(TAG,
    //                  " Failed to get SPIFFS partition information for '%s', reason '%s'",
    //                  partition_info->label,
    //                  esp_err_to_name(ret));
    //     } else {
    //         ESP_LOGI(TAG,
    //                  " Partition '%s' size, total: %d, used: %d",
    //                  partition_info->label,
    //                  total,
    //                  used);
    //     }
    // }

    // Get general partition info
    const char *storage = "storage";
    size_t total = 0, used = 0;
    auto ret = esp_spiffs_info(storage, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG,
                 " Failed to get SPIFFS partition information for '%s', reason '%s'",
                 storage,
                 esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, " Partition '%s' size, total: %d, used: %d", storage, total, used);
    }

    auto files = FSWrapper::listFiles(storage);

    for (const auto &info : files) {
        ESP_LOGI(TAG, " File: %s, size %ld", info.filename.c_str(), info.size);
    }
}

void HealthService::reportRAM() {
    ESP_LOGI(TAG, "RAM avaliable for default malloc in KB:");

    auto total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);

    multi_heap_info_t info = {};
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);

    ESP_LOGI(TAG,
             " total %f, allocated: %f, free: %f, used: %f, largest possible block to allocate: %f",
             total / 1024.0f,
             info.total_allocated_bytes / 1024.0f,
             info.total_free_bytes / 1024.0f,
             info.total_allocated_bytes / (float)total,
             info.largest_free_block / 1024.0f);

    ESP_LOGI(TAG,
             " total blocks %u, allocated blocks %u, free blocks %u",
             info.total_blocks,
             info.allocated_blocks,
             info.free_blocks);
}
void HealthService::reportCPU() {
    ESP_LOGI(TAG, "CPU:");
}

void HealthService::reportOS() {
    ESP_LOGI(TAG, "OS:");

    auto reset_reason = esp_reset_reason();
    ESP_LOGI(TAG, " Last reset reason %u", reset_reason);

    auto *idf_version = esp_get_idf_version();
    ESP_LOGI(TAG, " ESP-IDF used to compile: %s", idf_version);

    auto num_of_ticks = xTaskGetTickCount();
    ESP_LOGI(TAG, " Uptime %u ms", pdTICKS_TO_MS(num_of_ticks));

    auto num_of_tasks = uxTaskGetNumberOfTasks();
    ESP_LOGI(TAG, " Number of managed tasks %u", num_of_tasks);

// Get task info, if FreeRTOS is configured with that feature
#if configUSE_TRACE_FACILITY == 1 && configUSE_STATS_FORMATTING_FUNCTIONS == 1
    std::string buff;
    // Approximately 40 bytes per task should be sufficient.
    buff.reserve(40 * num_of_tasks);
    vTaskList(buff.data());
    ESP_LOGI(TAG, " Tasks:\n%s", buff.c_str());
#else
    ESP_LOGW(TAG, " OS not configured to report tasks list");
#endif
}

}  // namespace bk
