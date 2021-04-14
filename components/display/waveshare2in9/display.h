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
class Display : public AbstractTask {
 public:
    Display();
    virtual ~Display();

    void prepareCanvas();
    void invalidate();
    void drawKeypadData(const KeypadData &data);
    void drawWeatherData(const WeatherData &data);
    void drawGNSSData(const GNSSData &data);

    std::mutex &getBufferMutex() {
        return buffer_mutex_;
    }

    virtual void run() override;
    virtual void start() override;

    void draw();

 protected:
    static constexpr const char *TAG = "Display";

    Epd epd_;
    
    std::uint8_t *front_;
    std::uint8_t *back_;

    std::mutex buffer_mutex_;

    bool dirty_ = false;

    Paint paint_;
    
    void prettyClean();
    void drawTrackData();
};

}  // namespace bk
