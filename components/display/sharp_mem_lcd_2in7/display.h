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
    virtual void enqueueDraw(std::function<void(Paint &paint)> callback, const Rect &rect) = 0;

    /** @brief Draw stuff on back and front buffer
     * Used for static elements, like lines, which do not need invalidate
     */
    virtual void enqueueStaticDraw(std::function<void(Paint &paint)> callback,
                                   const Rect &rect) = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
};

class Display : public AbstractTask, public IDisplay {
 public:
    Display();
    virtual ~Display() = default;

    virtual void run() override;
    virtual void start() override;

    virtual void enqueueDraw(std::function<void(Paint &paint)> callback, const Rect &rect) override;

    virtual void enqueueStaticDraw(std::function<void(Paint &paint)> callback,
                                   const Rect &rect) override;

    virtual int getWidth() override;
    virtual int getHeight() override;

 protected:
    static constexpr const char *TAG = "Display";

    int width_;
    int height_;

    Driver driver_;

    std::vector<uint8_t> back_;

    std::recursive_mutex buffer_mutex_;

    Paint paint_;

    void draw();

    void prepareCanvas(const Rect &rect);
};

}  // namespace bk