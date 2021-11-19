#include "application.h"

// #include "ble_wrapper.h"
#include "event_dispatcher.h"
#include "fs_wrapper.h"
#include "gnss.h"
#include "health_service.h"
#include "keypad.h"
#include "layout_factory.h"
#include "root_window.h"
#include "time_service.hpp"
#include "weather.h"

#include <display.h>
#include <paint.h>
#include <driver.h>
#include <string.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include <nvs_flash.h>

static const char* TAG = "Application";

// void btTesting() {
//     esp_err_t ret;

//     /* Initialize NVS. */
//     ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);

//     bk::BLEWrapper ble;
//     ble.enable();

//     ESP_LOGI(TAG, "BLE enabled...");

//     while (true) {
//         HealthService::reportAll();
//         vTaskDelay(pdMS_TO_TICKS(2000));
//     }
// }

void StartApplication() {
    // btTesting();

    bk::Driver d;
    // d.clearDisplay();

    auto paint = d.getPaint();

    int cnt = 0;
    while (true) {
        HealthService::reportAll();
        vTaskDelay(pdMS_TO_TICKS(1000));

        d.clearDisplay();

        const int msg_size = 128;
        char message[msg_size];

        snprintf(message, msg_size, "BLAH: %d", cnt);
        cnt++;

        paint.DrawStringAt(50, 50, message, &Font24, COLORED);
        paint.DrawCircle(50, 50, 30, COLORED);
        paint.DrawFilledCircle(200, 200, 50, COLORED);
        paint.DrawFilledRectangle(100, 100, 150, 150, COLORED);
        paint.DrawVerticalLine(100, 100, 100, COLORED);

        d.refresh();
    }

    bk::FSWrapper::mountStorage();

    bk::Display display;

    display.start();

    bk::Weather weather;
    bk::GNSS gnss;
    bk::Keypad keypad;
    bk::TimeService time_service;

    bk::EventDispatcher dispatcher(
        weather.getQueue(), gnss.getQueue(), keypad.getQueue(), time_service.getQueue());

    time_service.setEventDispatcher(&dispatcher);
    // TODO: This is a dangling pointer - in theory
    // In practice application never ends
    bk::ActivityService::instance().setEventDispatcher(&dispatcher);

    weather.start();
    gnss.start();
    keypad.start();
    time_service.start();

    bk::LayoutFactory factory(&display, &dispatcher);

    bk::RootWindow root(&dispatcher, &factory);

    HealthService::reportAll();

    dispatcher.listenForEvents();
}