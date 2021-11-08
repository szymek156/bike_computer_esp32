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

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include <esp_log.h>

/** @brief show time spent on communicating with display HW */
// #define PERF

/** @brief Show areas which are invalidated*/
// #define DEBUG_RECTS

namespace bk {

// TODO: to be removed
static int rotate = ROTATE_270;

Display::Display()
    : AbstractTask(sizeof(DisplayData)),
      front_((uint8_t*)malloc(epd_.width / 8 * epd_.height)),
      back_((uint8_t*)malloc(epd_.width / 8 * epd_.height)),
      dirty_(false),
      paint_(back_, epd_.width, epd_.height) {
    memset(back_, 0xFF, epd_.width / 8 * epd_.height);
    memset(front_, 0xFF, epd_.width / 8 * epd_.height);
    paint_.SetRotate(rotate);
}

// void Display::drawTrackData() {
// }

// void Display::drawKeypadData(const KeypadData& data) {
//     if (data.lu_pressed) {
//         rotate = ROTATE_270;
//     } else if (data.ru_pressed) {
//         rotate = ROTATE_90;
//     }

//     paint_.SetRotate(rotate);

//     if (data.lu_pressed) {
//         paint_.DrawFilledCircle(210, 42, 10, COLORED);
//     }

//     if (data.ru_pressed) {
//         paint_.DrawFilledCircle(250, 42, 10, COLORED);
//     }

//     if (data.ld_pressed) {
//         paint_.DrawFilledCircle(210, 72, 10, COLORED);
//     }

//     if (data.rd_pressed) {
//         paint_.DrawFilledCircle(250, 72, 10, COLORED);
//     }
// }

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
    std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);

    prepareCanvas(rect);

    callback(paint_);

    invalidate();
}

void Display::enqueueStaticDraw(std::function<void(Paint& paint)> callback, const Rect& rect) {
    std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);

    enqueueDraw(callback, rect);
    swapBuffers();
    enqueueDraw(callback, rect);
}

int Display::getWidth() {
    return epd_.width;
}
int Display::getHeight() {
    return epd_.height;
}

void Display::draw() {
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    swapBuffers();

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

void Display::invalidate() {
    dirty_ = true;
}

void Display::prepareCanvas(const Rect& rect) {
    paint_.DrawFilledRectangle(rect.x0, rect.y0, rect.x1, rect.y1, UNCOLORED);

#if defined(DEBUG_RECTS)
    paint_.DrawFilledRectangle(rect.x0, rect.y0, rect.x1, rect.y1, COLORED);
#endif
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

void Display::swapBuffers() {
    std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);

    // Dirty means, new data is ready to be shown
    if (dirty_) {
        // TODO: swapping buffers causes show of old data, in case of
        // views updating less often than refresh-rate of display.
        // For example weather which seems to spam every 2 seconds,
        // in result, on display there is flickering new and old value

        // So instead of swapping buffers, everything what is on back
        // (always holds newest data) is copyied to front_.
        // Wonder if there is any way to avoid memcpy, and still use swap.

        // std::swap(front_, back_);
        // paint_.SetImage(back_);

        memcpy(front_, back_, epd_.width / 8 * epd_.height);
        dirty_ = false;
    }
}
}  // namespace bk
