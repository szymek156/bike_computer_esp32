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

#include <random>

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

void Display::drawGPSData() {
}

void Display::drawWeatherData() {
}

void Display::run() {
    prettyClean();

    if (_epd.Init(lut_partial_update) != 0) {
        // _log->error("e-Paper init failed\n");
        return;
    }

    paint.SetRotate(ROTATE_90);
    paint.Clear(UNCOLORED);

    int d = 0;
    while (true) {
        d++;
        paint.Clear(UNCOLORED);

        const int strSize = 128;
        char message[strSize];

        snprintf(message, strSize, "It works!: %d", d);
        paint.DrawStringAt(0, 78, message, &Font20, COLORED);

        _epd.SetFrameMemory(paint.GetImage(), 0, 0, _epd.width, _epd.height);
        _epd.DisplayFrame();
    }
}

}  // namespace bk
