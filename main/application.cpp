#include "application.h"

#include "display.h"
#include "gnss.h"
#include "sensor_data.h"
#include "weather.h"

#include <chrono>
#include <iostream>
#include <thread>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include <esp_log.h>

void StartApplication() {
    static const char* TAG = "Broker";

    bk::Weather weather;
    bk::GNSS gnss;
    bk::Display display;

    auto total_queue_size = bk::Weather::QUEUE_SIZE + bk::GNSS::QUEUE_SIZE;

    QueueSetHandle_t poll = xQueueCreateSet(total_queue_size);

    configASSERT(xQueueAddToSet(weather.getQueue(), poll) == pdPASS);
    configASSERT(xQueueAddToSet(gnss.getQueue(), poll) == pdPASS);

    weather.start();
    gnss.start();

    ESP_LOGI(TAG, "Polling the queue set");
    while (true) {
        ESP_LOGI(TAG, "Select...");
        // QueueSetMemberHandle_t* queue = xQueueSelectFromSet(poll, portMAX_DELAY);
        QueueSetMemberHandle_t queue = nullptr;

        while ((queue = xQueueSelectFromSet(poll, pdMS_TO_TICKS(100))) != nullptr) {
            if (queue == weather.getQueue()) {
                ESP_LOGV(TAG, "Got data from Weather");
                bk::WeatherData data;
                xQueueReceive(queue, &data, 0);

                display.drawWeatherData(data);
                // display.invalidate();
            } else if (queue == gnss.getQueue()) {
                ESP_LOGV(TAG, "Got data from GNSS");
                bk::GNSSData data;
                xQueueReceive(queue, &data, 0);

                display.drawGNSSData(data);
                // display.invalidate();
            }
        }

        ESP_LOGV(TAG, "invalidate");
        display.invalidate();
    }
}