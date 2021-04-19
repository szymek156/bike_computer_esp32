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

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

// #define PERF

namespace bk {

// TODO: to be removed
static int rotate = ROTATE_270;

Display::Display()
    : AbstractTask(sizeof(DisplayData)),
      front_((uint8_t*)calloc(epd_.width / 8 * epd_.height, sizeof(std::uint8_t))),
      back_((uint8_t*)calloc(epd_.width / 8 * epd_.height, sizeof(std::uint8_t))),
      paint_(back_, epd_.width, epd_.height) {
    memset(back_, 0xFF, epd_.width / 8 * epd_.height);
    memset(front_, 0xFF, epd_.width / 8 * epd_.height);
    paint_.SetRotate(rotate);
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
    if (data.lu_pressed) {
        rotate = ROTATE_270;
    } else if (data.ru_pressed) {
        rotate = ROTATE_90;
    }

    paint_.SetRotate(rotate);

    if (data.lu_pressed) {
        paint_.DrawFilledCircle(210, 42, 10, COLORED);
    }

    if (data.ru_pressed) {
        paint_.DrawFilledCircle(250, 42, 10, COLORED);
    }

    if (data.ld_pressed) {
        paint_.DrawFilledCircle(210, 72, 10, COLORED);
    }

    if (data.rd_pressed) {
        paint_.DrawFilledCircle(250, 72, 10, COLORED);
    }
}

void Display::drawWeatherData(const WeatherData& data) {
    const int msg_size = 128;
    char message[msg_size];

    snprintf(message, msg_size, "SENSOR:");
    paint_.DrawStringAt(0, 78, message, &Font20, COLORED);

    snprintf(message, msg_size, "TMP   %5.2f", data.temp_c);
    paint_.DrawStringAt(0, 95, message, &Font16, COLORED);

    snprintf(message, msg_size, "ALT  %7.2f", data.altitude_m);
    paint_.DrawStringAt(0, 110, message, &Font16, COLORED);
}

void Display::invalidate() {
    dirty_ = true;
}

void Display::prepareCanvas(const Rect& rect) {
    if (dirty_) {
        // Display did not get data yet, clear back buffer
        // to draw newer stuff

        paint_.DrawFilledRectangle(rect.x0, rect.y0, rect.x1, rect.y1, UNCOLORED);
    }
    // paint_.DrawFilledRectangle(rect.x0, rect.y0, rect.x1, rect.y1, COLORED);
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

void Display::enqueueDraw(std::function<void(Paint& paint)> callback, const Rect& rect) {
    std::lock_guard<std::mutex> lock(buffer_mutex_);

    prepareCanvas(rect);

    callback(paint_);

    invalidate();
}

void Display::draw() {
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);

        if (dirty_) {
            std::swap(front_, back_);
            // memset(back_, 0xFF, epd_.width / 8 * epd_.height);

            paint_.SetImage(back_);
            // dirty_ = false;
        }
    }

#if defined(PERF)
    std::chrono::time_point<std::chrono::system_clock> after_swap =
        std::chrono::system_clock::now();
#endif

    epd_.SetFrameMemory(front_, 0, 0, epd_.width, epd_.height);

#if defined(PERF)
    std::chrono::time_point<std::chrono::system_clock> after_set_frame =
        std::chrono::system_clock::now();
#endif

    epd_.DisplayFrame();

    std::chrono::time_point<std::chrono::system_clock> after_display =
        std::chrono::system_clock::now();

#if defined(PERF)
    ESP_LOGD(
        TAG,
        "swap %llu, set frame %llu, display frame %llu, total %llu",
        std::chrono::duration_cast<std::chrono::milliseconds>(after_swap - start).count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(after_set_frame - after_swap).count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(after_display - after_set_frame)
            .count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(after_display - start).count());
#endif

    auto to_sleep = std::max(
        1000 - (int)std::chrono::duration_cast<std::chrono::milliseconds>(after_display - start)
                   .count(),
        0);

    ESP_LOGD(TAG, "To sleep %d", to_sleep);
    vTaskDelay(pdMS_TO_TICKS(to_sleep));
}
}  // namespace bk
