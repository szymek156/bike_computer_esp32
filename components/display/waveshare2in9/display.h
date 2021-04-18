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

#include <mutex>

#include <epd2in9.h>
#include <epdpaint.h>

namespace bk {

struct Rect {
    int x0;
    int y0;
    int x1;
    int y1;
};

class IDisplay {
 public:
    virtual ~IDisplay() = default;

    virtual void prepareCanvas(const Rect *rect) = 0;
    virtual void invalidate() = 0;
    virtual std::mutex &getBufferMutex() = 0;
    virtual Paint getPaint() = 0;
};

const int COLORED = 0;
const int UNCOLORED = 1;

class Display : public AbstractTask, public IDisplay {
 public:
    Display();
    virtual ~Display() = default;

    virtual void prepareCanvas(const Rect *rect = nullptr) override;
    virtual void invalidate() override;

    virtual std::mutex &getBufferMutex() override {
        return buffer_mutex_;
    }

    virtual Paint getPaint() override;

    virtual void run() override;
    virtual void start() override;

    void draw();

    void drawKeypadData(const KeypadData &data);
    void drawWeatherData(const WeatherData &data);
    void drawTrackData();

 protected:
    static constexpr const char *TAG = "Display";

    Epd epd_;

    std::uint8_t *front_;
    std::uint8_t *back_;

    std::mutex buffer_mutex_;

    bool dirty_ = false;

    Paint paint_;

    void prettyClean();
};

}  // namespace bk
