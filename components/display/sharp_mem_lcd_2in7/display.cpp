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

/** @brief show time spent on communicating with display HW */
#define PERF

/** @brief Show areas which are invalidated*/
// #define DEBUG_RECTS

namespace bk {

Display::Display()
    : AbstractTask(sizeof(DisplayData)),
      width_(400),
      height_(240),
      driver_(Driver(width_, height_)),
      // Back holds raw pixel data
      // Template argument deduction my ass
      back_(std::vector<uint8_t>((width_ * height_) / 8, 0)),
      paint_(back_.data(), width_, height_, Endian::Little) {
}

void Display::start() {
    memset(back_.data(), 0xFF, back_.size());
    driver_.clearDisplay();

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

    // TODO: there is a great opportunity to improve the drawing, by
    // calculating dirty regions, clearing only that area, and passing only
    // dirty rectangles to the device, not the whole frame. Since I abandon developing
    // on the e-ink for now, this will be not done here.
    // But I plan to do this for memory display.
    prepareCanvas(rect);

    callback(paint_);
}

void Display::enqueueStaticDraw(std::function<void(Paint& paint)> callback, const Rect& rect) {
    std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);

    enqueueDraw(callback, rect);
}

int Display::getWidth() {
    return width_;
}
int Display::getHeight() {
    return height_;
}

void Display::draw() {
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

#if defined(PERF)
    std::chrono::time_point<std::chrono::system_clock> after_swap =
        std::chrono::system_clock::now();
#endif

    // TODO: check if dirty, if yes, copy, transfer
    // else just toggle vcom - iff, one second passed

    {
        // This works as long as driver_.setFrame and driver_.draw()
        // are called from the same thread.
        // If that's gonna change, then driver should keep a mutex
        // for it's internal "front" buffer
        // Or merge setFrame() and draw() to one method -> draw(back_)

        // This mutex keeps synchronization, between display task, and
        // other tasks (most likely event dispatcher), that draw on the
        // back buffer.
        // Only setFrame needs to be locked - it's a memcpy of back_
        // to internal front_ buffer - it takes less than one millisecond
        // draw is a transmision of the front buffer over SPI - that's
        // 10ms. So this lock is held for very short period of time
        std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);
        driver_.setFrame(back_.data());
    }

#if defined(PERF)
    std::chrono::time_point<std::chrono::system_clock> after_set_frame =
        std::chrono::system_clock::now();
#endif

    driver_.draw();

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

void Display::prepareCanvas(const Rect& rect) {
    paint_.DrawFilledRectangle(rect.x0, rect.y0, rect.x1, rect.y1, UNCOLORED);

#if defined(DEBUG_RECTS)
    paint_.DrawFilledRectangle(rect.x0, rect.y0, rect.x1, rect.y1, COLORED);
#endif
}

}  // namespace bk
