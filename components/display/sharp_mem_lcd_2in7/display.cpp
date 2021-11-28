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
      dirty_(false),
      driver_(Driver(width_, height_)),
      vcom_watchdog_(std::chrono::system_clock::now()),
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

void Display::enqueueDraw(std::function<void(Paint& paint)> callback) {
    std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);

    // TODO: there is a great opportunity to improve the drawing, by
    // calculating dirty regions, clearing only that area, and passing only
    // dirty rectangles to the device, not the whole frame.

    {
        auto recorder = paint_.RecordDirtyRegions();
        callback(paint_);
    }

    auto regions = paint_.GetRegions();

    for (auto &region : regions) {
        clearRegion(region);
    }

    // clearRegion(rect);

    callback(paint_);

    setDirty(true);
}

int Display::getWidth() {
    return width_;
}
int Display::getHeight() {
    return height_;
}

void Display::setDirty(bool dirty) {
    std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);

    dirty_ = dirty;
}

bool Display::isDirty() {
    std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);

    return dirty_;
}
void Display::draw() {
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    bool dirty = false;
    {
        // This works as long as driver_.setFrame and driver_.draw()
        // are called from the same thread.
        // If that's gonna change, then driver should keep a mutex
        // for it's internal "front" buffer
        // Or merge setFrame() and draw() to one method -> draw(back_)

        // This mutex keeps synchronization, between display task, and
        // other tasks (most likely event dispatcher), that draw on the
        // back_ buffer.
        // Only setFrame needs to be locked - it's a memcpy of back_
        // to internal front_ buffer - it takes less than one millisecond
        // draw is a transmission of the front buffer over SPI - that's
        // 10ms. So this lock is held for very short period of time
        std::lock_guard<std::recursive_mutex> lock(buffer_mutex_);
        dirty = isDirty();

        if (dirty) {
            driver_.setFrame(back_.data());
            setDirty(false);
        }
    }

#if defined(PERF)
    std::chrono::time_point<std::chrono::system_clock> after_set_frame =
        std::chrono::system_clock::now();
#endif

    // Out of the mutex guard, task does not block others to write on back_
    // but dirty must be checked once again.
    if (dirty) {
        // Draw will toggle the vcom
        driver_.draw();
        vcom_watchdog_ = start;

    } else {
        // Nothing to be drawn, check if it's time to toggle the vcom
        // It's not clear from the docs, but looks like it should be
        // done every 1-30 seconds if image is static
        // Having to_sleep set to 500ms this will effectively refresh every 1.5 second
        // TODO: should this go to the driver?
        if (std::chrono::duration_cast<std::chrono::seconds>(start - vcom_watchdog_) >
            std::chrono::seconds(1)) {
            ESP_LOGD(TAG, "Toggling VCOM");
            driver_.toggleVcom();
            vcom_watchdog_ = start;
        }
    }

    std::chrono::time_point<std::chrono::system_clock> after_display =
        std::chrono::system_clock::now();

#if defined(PERF)
    ESP_LOGD(TAG,
             "set frame %llu, display frame %llu, total %llu",
             std::chrono::duration_cast<std::chrono::milliseconds>(after_set_frame - start).count(),
             std::chrono::duration_cast<std::chrono::milliseconds>(after_display - after_set_frame)
                 .count(),
             std::chrono::duration_cast<std::chrono::milliseconds>(after_display - start).count());
#endif

    auto to_sleep = std::max(
        500 - (int)std::chrono::duration_cast<std::chrono::milliseconds>(after_display - start)
                  .count(),
        0);

    ESP_LOGD(TAG, "To sleep %d", to_sleep);
    vTaskDelay(pdMS_TO_TICKS(to_sleep));
}

void Display::clearRegion(const Rect& rect) {
#if defined(DEBUG_RECTS)
    auto color = COLORED;
#else
    auto color = UNCOLORED;
#endif

    paint_.DrawFilledRectangle(rect.x0, rect.y0, rect.x1, rect.y1, color);
}

}  // namespace bk
