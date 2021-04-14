/*
 * Display.cpp
 *
 *  Created on: May 5, 2018
 *      Author: Szymon
 *
 *                 Version 2, December 2004
 *
 *       Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *          DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 * 0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include "display.h"

#include "sensor_data.h"

#include <chrono>
#include <cstring>
#include <random>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

namespace bk {

#define COLORED 0
#define UNCOLORED 1

// TODO: to be removed
static int rotate = ROTATE_270;

Display::Display()
    : AbstractTask(sizeof(DisplayData)),
      front_((uint8_t*)calloc(epd_.width / 8 * epd_.height, sizeof(std::uint8_t))),
      back_((uint8_t*)calloc(epd_.width / 8 * epd_.height, sizeof(std::uint8_t))) {
    memset(back_, 0xFF, epd_.width / 8 * epd_.height);
    memset(front_, 0xFF, epd_.width / 8 * epd_.height);
}

Display::~Display() {
}

void Display::prettyClean() {
    if (epd_.Init(lut_full_update) != 0) {
        ESP_LOGE(TAG, "e-Paper initialization failed");
        return;
    }

    epd_.ClearFrameMemory(0xFF);
    epd_.DisplayFrame();
    epd_.ClearFrameMemory(0xFF);
    epd_.DisplayFrame();
}

void Display::drawTrackData() {
}

void Display::drawKeypadData(const KeypadData& data) {
    Paint paint(back_, epd_.width, epd_.height);

    if (data.lu_pressed) {
        rotate = ROTATE_270;
    } else if (data.ru_pressed) {
        rotate = ROTATE_90;
    }

    paint.SetRotate(rotate);

    if (data.lu_pressed) {
        paint.DrawFilledCircle(210, 42, 10, COLORED);
    }

    if (data.ru_pressed) {
        paint.DrawFilledCircle(250, 42, 10, COLORED);
    }

    if (data.ld_pressed) {
        paint.DrawFilledCircle(210, 72, 10, COLORED);
    }

    if (data.rd_pressed) {
        paint.DrawFilledCircle(250, 72, 10, COLORED);
    }
}

void Display::drawGNSSData(const GNSSData& data) {
    Paint paint(back_, epd_.width, epd_.height);
    paint.SetRotate(rotate);

    const int msg_size = 128;
    char message[msg_size];

    strftime(message, msg_size, "%D %T", &(data.date_time));
    paint.DrawStringAt(0, 4, message, &Font24, COLORED);

    snprintf(message, msg_size, "SPD %5.2f km/h", data.speed_kmh);
    paint.DrawStringAt(0, 32, message, &Font20, COLORED);

    snprintf(message, msg_size, "ALT %7.2f mnpm", data.altitude);
    paint.DrawStringAt(0, 54, message, &Font16, COLORED);

    snprintf(message, msg_size, "SAT's in view  %2d", data.sats_in_view);
    paint.DrawStringAt(170, 95, message, &Font12, COLORED);

    snprintf(message, msg_size, "SAT's tracked  %2d", data.sats_tracked);
    paint.DrawStringAt(170, 105, message, &Font12, COLORED);

    snprintf(message, msg_size, "GPS fix status %2d", data.fix_status);
    paint.DrawStringAt(170, 115, message, &Font12, COLORED);
}

void Display::drawWeatherData(const WeatherData& data) {
    Paint paint(back_, epd_.width, epd_.height);
    paint.SetRotate(rotate);

    const int msg_size = 128;
    char message[msg_size];

    snprintf(message, msg_size, "SENSOR:");
    paint.DrawStringAt(0, 78, message, &Font20, COLORED);

    snprintf(message, msg_size, "TMP   %5.2f", data.temp_c);
    paint.DrawStringAt(0, 95, message, &Font16, COLORED);

    snprintf(message, msg_size, "ALT  %7.2f", data.altitude_m);
    paint.DrawStringAt(0, 110, message, &Font16, COLORED);
}

void Display::invalidate() {
    dirty_ = true;
}

void Display::prepareCanvas() {
    if (dirty_) {
        // Display did not get data yet, clear back buffer
        // to draw newer stuff
        memset(back_, 0xFF, epd_.width / 8 * epd_.height);
    }
}

void Display::start() {
    prettyClean();

    if (epd_.Init(lut_partial_update) != 0) {
        ESP_LOGE(TAG, "e-Paper init failed");
        return;
    }

    start_execution(TAG);
}

void Display::run() {
    // Important as fuk
    vTaskPrioritySet(task_, configMAX_PRIORITIES - 1);

    while (true) {
        ESP_LOGD(TAG, "Waiting for an event");
        draw();
    }
}

void Display::draw() {
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);

        if (dirty_) {
            std::swap(front_, back_);
            memset(back_, 0xFF, epd_.width / 8 * epd_.height);

            dirty_ = false;
        }
    }

    std::chrono::time_point<std::chrono::system_clock> after_swap =
        std::chrono::system_clock::now();

    epd_.SetFrameMemory(front_, 0, 0, epd_.width, epd_.height);

    std::chrono::time_point<std::chrono::system_clock> after_set_frame =
        std::chrono::system_clock::now();

    epd_.DisplayFrame();

    std::chrono::time_point<std::chrono::system_clock> after_display =
        std::chrono::system_clock::now();

    ESP_LOGD(
        TAG,
        "swap %llu, set frame %llu, display frame %llu, total %llu",
        std::chrono::duration_cast<std::chrono::milliseconds>(after_swap - start).count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(after_set_frame - after_swap).count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(after_display - after_set_frame)
            .count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(after_display - start).count());

    auto to_sleep = std::max(
        1000 - (int)std::chrono::duration_cast<std::chrono::milliseconds>(after_display - start)
                   .count(),
        0);

    ESP_LOGD(TAG, "To sleep %d", to_sleep);
    vTaskDelay(pdMS_TO_TICKS(to_sleep));
}
}  // namespace bk
