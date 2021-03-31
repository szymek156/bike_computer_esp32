#include "application.h"

#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "gnss.h"
#include "keypad.h"
#include "sensor_data.h"
#include "weather.h"

#include <chrono>
#include <iostream>
#include <thread>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

/** @brief Code is unused due to a bug in freertos/queue.c implementation.
 * QueueSet is not working for queues which do have "overwrite" policy.
 * Version 10.3 has a fix. ESP32 uses for now 10.2
 */
// void pollQueues() {
//     auto total_queue_size = bk::Weather::QUEUE_SIZE + bk::GNSS::QUEUE_SIZE;

//     QueueSetHandle_t poll = xQueueCreateSet(total_queue_size);

//     configASSERT(xQueueAddToSet(weather.getQueue(), poll) == pdPASS);
//     configASSERT(xQueueAddToSet(gnss.getQueue(), poll) == pdPASS);

//     ESP_LOGI(TAG, "Polling the queue set");
//     while (true) {
//         ESP_LOGI(TAG, "Select...");
//         QueueSetMemberHandle_t queue = xQueueSelectFromSet(poll, portMAX_DELAY);
//         // QueueSetMemberHandle_t queue = nullptr;

//         // while ((queue = xQueueSelectFromSet(poll, pdMS_TO_TICKS(100))) != nullptr) {
//         if (queue == weather.getQueue()) {
//             ESP_LOGV(TAG, "Got data from Weather");
//             bk::WeatherData data;
//             configASSERT(xQueueReceive(queue, &data, 0) == pdPASS);

//             display.drawWeatherData(data);
//             // display.invalidate();
//         } else if (queue == gnss.getQueue()) {
//             ESP_LOGV(TAG, "Got data from GNSS");
//             bk::GNSSData data;
//             configASSERT(xQueueReceive(queue, &data, 0) == pdPASS);

//             display.drawGNSSData(data);
//             // display.invalidate();
//         } else {
//             ESP_LOGV(TAG, "else");
//             configASSERT(false);
//         }
//         // }

//         // ESP_LOGV(TAG, "leave while");

//         ESP_LOGV(TAG, "invalidate");
//         display.invalidate();
//     }
// }

void setupGPIO() {
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_SEL_21 | GPIO_SEL_22 | GPIO_SEL_23 | GPIO_SEL_34;

    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&io_conf);
}

void StartApplication() {
    static const char *TAG = "Broker";

    setupGPIO();

    bk::Weather weather;
    bk::GNSS gnss;
    bk::Display display;
    bk::Keypad keypad;

    weather.start();
    gnss.start();
    keypad.start();

    auto *weather_q = weather.getQueue();
    auto *gnss_q = gnss.getQueue();
    auto *keypad_q = keypad.getQueue();

    bk::WeatherData weather_data = {};
    bk::GNSSData gnss_data = {};
    bk::KeypadData keypad_data = {};

    static const TickType_t TIMEOUT = pdMS_TO_TICKS(200);

    while (true) {
        bool to_invalidate = false;

        if (xQueueReceive(keypad_q, &keypad_data, TIMEOUT) == pdPASS) {
            ESP_LOGV(TAG, "Got data from Keypad");
        }

        if (xQueueReceive(weather_q, &weather_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got data from Weather");
            to_invalidate = true;
        }

        if (xQueueReceive(gnss_q, &gnss_data, 0) == pdPASS) {
            ESP_LOGV(TAG, "Got data from GNSS");
            to_invalidate = true;
        }

        if (to_invalidate) {
            display.drawWeatherData(weather_data);
            display.drawGNSSData(gnss_data);

            ESP_LOGV(TAG, "invalidate");
            display.invalidate();
        }
    }
}