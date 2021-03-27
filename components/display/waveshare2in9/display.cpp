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

#include <chrono>
#include <random>
#include <thread>

#include <epd2in9.h>
#include <epdpaint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace bk {
static Epd _epd;
unsigned char* frame_buffer = (unsigned char*)malloc(_epd.width / 8 * _epd.height);

static Paint paint(frame_buffer, _epd.width, _epd.height);

#define COLORED 0
#define UNCOLORED 1

Display::Display() {
    prettyClean();

    if (_epd.Init(lut_partial_update) != 0) {
        // _log->error("e-Paper init failed\n");
        return;
    }

    paint.SetRotate(ROTATE_270);
    paint.Clear(UNCOLORED);
}

Display::~Display() {
}

void Display::prettyClean() {
    if (_epd.Init(lut_full_update) != 0) {
        // log->error("e-Paper init failed\n");
        return;
    }

    _epd.ClearFrameMemory(0xFF);
    _epd.DisplayFrame();
    _epd.ClearFrameMemory(0xFF);
    _epd.DisplayFrame();
}

void Display::drawTrackData() {
}

void Display::drawGNSSData(const GNSSData &data) {
    const int msg_size = 128;
    char message[msg_size];

    strftime(message, msg_size, "%D %T", &(data.date_time));
    paint.DrawStringAt(0, 4, message, &Font24, COLORED);

    snprintf(message, msg_size, "SPD %5.2f km/h", data.speed_kmh);
    paint.DrawStringAt(0, 32, message, &Font20, COLORED);

    snprintf(message, msg_size,   "ALT %7.2f mnpm", data.altitude);
    paint.DrawStringAt(0, 54, message, &Font16, COLORED);

    snprintf(message, msg_size, "SAT's in view  %2d", data.sats_in_view);
    paint.DrawStringAt(170, 95, message, &Font12, COLORED);

    snprintf(message, msg_size, "SAT's tracked  %2d", data.sats_tracked);
    paint.DrawStringAt(170, 105, message, &Font12, COLORED);

    snprintf(message, msg_size, "GPS fix status %2d", data.fix_status);
    paint.DrawStringAt(170, 115, message, &Font12, COLORED);

}

void Display::drawWeatherData(const WeatherData& data) {
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
    _epd.SetFrameMemory(paint.GetImage(), 0, 0, _epd.width, _epd.height);
    _epd.DisplayFrame();
    paint.Clear(UNCOLORED);
}

}  // namespace bk
