/**
 *  @filename   :   paint.h
 *  @brief      :   Header file for paint.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     July 28 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef EPDPAINT_H
#define EPDPAINT_H

// Display orientation
#define ROTATE_0 0
#define ROTATE_90 1
#define ROTATE_180 2
#define ROTATE_270 3

#define COLORED 0
#define UNCOLORED 1

#include "fonts.h"

#include <vector>

enum class Endian { Big, Little };

/** @brief Rectangle describing dirty region */
struct Rect {
    int x0;
    int y0;
    int x1;
    int y1;
};

using Regions = std::vector<Rect>;

class Paint {
 public:
    Paint(unsigned char* image, int width, int height, Endian endian);
    ~Paint();
    void Clear(int colored);
    int GetWidth(void);
    void SetWidth(int width);
    int GetHeight(void);
    void SetHeight(int height);
    int GetRotate(void);
    void SetRotate(int rotate);
    unsigned char* GetImage(void);
    void SetImage(unsigned char* image);

    void DrawStringAt(int x, int y, const char* text, sFONT* font, int colored);
    void DrawLine(int x0, int y0, int x1, int y1, int colored);
    void DrawHorizontalLine(int x, int y, int width, int colored);
    void DrawVerticalLine(int x, int y, int height, int colored);
    void DrawRectangle(int x0, int y0, int x1, int y1, int colored);
    void DrawFilledRectangle(int x0, int y0, int x1, int y1, int colored);
    void DrawCircle(int x, int y, int radius, int colored);
    void DrawFilledCircle(int x, int y, int radius, int colored);

    /** @brief Gets recorded regions */
    Regions GetRegions();

    class PaintGuard {
     public:
        PaintGuard(Paint& paint);
        ~PaintGuard();

     private:
        Paint& paint_;
    };

    /** @brief Returns a recorder guard object, Paint will collect dirty regions
     * for all subsequent drawing calls as long as the guard is in scope.
     * During that time Paint class does not alter any buffer data, it just calculates which region
     * would be changed.
     */
    PaintGuard RecordDirtyRegions();

 private:
    void DrawAbsolutePixel(int x, int y, int colored);
    void DrawPixel(int x, int y, int colored);
    void DrawCharAt(int x, int y, char ascii_char, sFONT* font, int colored);

    void SetRecordDirtyRegions(bool record);

    unsigned char* image;
    int width;
    int height;
    int rotate;
    /** @brief Defines how pixels should be stored in the buffer
     * Little means leftmost pixel first, used by Sharp memory buffer
     * Big is other way around, used by E-Ink display
     */
    Endian endian_;
    class PaintRecorder {
     public:
        PaintRecorder() = default;
        ~PaintRecorder() = default;

        void RecordCircle(int x, int y, int radius);
        void RecordRectangle(int x0, int y0, int x1, int y1);

        void RecordLine(int x0, int y0, int x1, int y1);
        void RecordHorizontalLine(int x, int y, int width);
        void RecordVerticalLine(int x, int y, int height);
        void RecordStringAt(int x, int y, const char* text, sFONT* font);

        void SetRecord(bool record);

        inline bool IsRecording();

        Regions GetRegions();

     private:
        bool record_ = false;
        Regions regions_;
    };

    PaintRecorder recorder_;
};

#endif

/* END OF FILE */
