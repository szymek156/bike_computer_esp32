/*
 * Display.h
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

#pragma once
#include "abstract_task.h"
#include "sensor_data.h"

#include <functional>
#include <mutex>
#include <vector>
#include <chrono>

#include <driver.h>
#include <paint.h>

namespace bk {

class IDisplay {
 public:
    virtual ~IDisplay() = default;

    /** @brief Draw stuff on back buffer
     * @param callback defines what to draw
     * @param rect defines area where it should be drawn
     */
    virtual void enqueueDraw(std::function<void(Paint &paint)> callback) = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

    virtual void clearRegion(const Rect &rect) = 0;
};

class Display : public AbstractTask, public IDisplay {
 public:
    Display();
    virtual ~Display() = default;

    virtual void run() override;
    virtual void start() override;

    virtual void enqueueDraw(std::function<void(Paint &paint)> callback) override;

    virtual int getWidth() override;
    virtual int getHeight() override;

    virtual void clearRegion(const Rect &rect) override;

    /** @brief Sets dirty flag. If true, back_ buffer will be transferred to the device */
    void setDirty(bool dirty);

    bool isDirty();

 protected:
    static constexpr const char *TAG = "Display";

    int width_;
    int height_;
    bool dirty_;

    Driver driver_;

    std::chrono::time_point<std::chrono::system_clock> vcom_watchdog_;

    std::vector<uint8_t> back_;

    std::recursive_mutex buffer_mutex_;


    Paint paint_;

    void draw();
};

}  // namespace bk
