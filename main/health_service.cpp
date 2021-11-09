#include "health_service.h"

#include <string>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_log.h>
#include <esp_spiffs.h>

void HealthService::report() {
    reportFS();
    reportRAM();
    reportCPU();
    reportOS();
}

void HealthService::reportFS() {
    ESP_LOGI(TAG, "Filesystem:");

    size_t total = 0, used = 0;
    auto ret = esp_spiffs_info("storage", &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, " Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, " Partition size: total: %d, used: %d", total, used);
    }
}

void HealthService::reportRAM() {
    ESP_LOGI(TAG, "RAM:");
}
void HealthService::reportCPU() {
    ESP_LOGI(TAG, "CPU:");
}
void HealthService::reportOS() {
    ESP_LOGI(TAG, "OS:");

    auto num_of_ticks = xTaskGetTickCount();

    ESP_LOGI(TAG, " Uptime %u ms", pdTICKS_TO_MS(num_of_ticks));

    auto num_of_tasks = uxTaskGetNumberOfTasks();
    ESP_LOGI(TAG, " Number of managed tasks %u", num_of_tasks);

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
