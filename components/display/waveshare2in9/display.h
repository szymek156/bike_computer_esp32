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
#include "sensor_data.h"
namespace bk {
class Display {
 public:
    Display();
    virtual ~Display();

    void invalidate();
    void drawKeypadData(const KeypadData &data);
    void drawWeatherData(const WeatherData &data);
    void drawGNSSData(const GNSSData &data);

 protected:
    void prettyClean();
    void drawTrackData();
};

}  // namespace bk
