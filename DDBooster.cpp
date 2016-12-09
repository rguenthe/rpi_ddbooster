/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Daniel Wiese
 * Copyright (c) 2016 Richard Guenther
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>
#include <stdio.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "DDBooster.h"

#define BOOSTER_CMD_DELAY    500
#define BOOSTER_LED_DELAY    30

#define BOOSTER_SETRGB       0xA1
#define BOOSTER_SETRGBW      0xA2
#define BOOSTER_SETHSV       0xA3
#define BOOSTER_SETLED       0xA4
#define BOOSTER_SETALL       0xA5
#define BOOSTER_SETRANGE     0xA6
#define BOOSTER_SETRAINBOW   0xA7
#define BOOSTER_GRADIENT     0xA8

#define BOOSTER_INIT         0xB1
#define BOOSTER_SHOW         0xB2
#define BOOSTER_SHIFTUP      0xB3
#define BOOSTER_SHIFTDOWN    0xB4
#define BOOSTER_COPYLED      0xB5
#define BOOSTER_REPEAT       0xB6

#define BOOSTER_RGBORDER     0xC1

DDBooster::DDBooster() : _resetPin(0xFF), _lastIndex(0) {
    // initialize SPI at 12MHz, MSB first, mode 0
    _fd = wiringPiSPISetup(0, 12000000);
}

void DDBooster::configurePins(uint8_t resetPin) {
    _resetPin = resetPin;
    pinMode(_resetPin, OUTPUT);
}

void DDBooster::init(uint16_t ledCount, LedType ledType, LedColorOrder colorOrder) {
    // DD Booster expects the number of LEDs to be an even value (rounded up).
    // 256 is defined as 0 to fit in one byte.
    if (ledCount > 256) {
        ledCount = 256;
    }

    _lastIndex = ledCount - 1;

    uint8_t buffer[4];
    buffer[0] = BOOSTER_INIT;
    buffer[1] = ledCount + (ledCount & 1);
    buffer[2] = ledType;
    wiringPiSPIDataRW(0, buffer, 3);

    if (ledType == LED_RGB && colorOrder != ORDER_GRB) {
        buffer[0] = BOOSTER_RGBORDER;
        buffer[1] = 3;
        buffer[2] = 2;
        buffer[3] = 1;
        wiringPiSPIDataRW(0, buffer, 4);
    }

    // a delay after init is not documented, but seems to be necessary
    delay(20);
}

void DDBooster::reset() {
    if (_resetPin < 0xFF) {
        digitalWrite(_resetPin, LOW);
        delay(100);
        digitalWrite(_resetPin, HIGH);
        delay(100);
    }
}

void DDBooster::setRGB(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t cmd[4];
    cmd[0] = BOOSTER_SETRGB;
    cmd[1] = r;
    cmd[2] = g;
    cmd[3] = b;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::setRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    uint8_t cmd[5];
    cmd[0] = BOOSTER_SETRGBW;
    cmd[1] = r;
    cmd[2] = g;
    cmd[3] = b;
    cmd[4] = w;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::setHSV(uint16_t h, uint8_t s, uint8_t v) {
    if (h > 359) {
        h = 359;
    }
    uint8_t cmd[5];
    cmd[0] = BOOSTER_SETHSV;
    cmd[1] = h & 0xFF;
    cmd[2] = h >> 8;
    cmd[3] = s;
    cmd[4] = v;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::setLED(uint8_t index) {
    if (index > _lastIndex) {
        return;
    }
    uint8_t cmd[2];
    cmd[0] = BOOSTER_SETLED;
    cmd[1] = index;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::clearLED(uint8_t index) {
    if (index > _lastIndex) {
        return;
    }
    // optimization by sending two commands in one transaction
    uint8_t cmd[6];
    cmd[0] = BOOSTER_SETRGB;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = BOOSTER_SETLED;
    cmd[5] = index;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::setAll() {
    uint8_t cmd[1];
    cmd[0] = BOOSTER_SETALL;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::clearAll() {
    // optimization by sending two commands in one transaction
    uint8_t cmd[5];
    cmd[0] = BOOSTER_SETRGB;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = BOOSTER_SETALL;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::setRange(uint8_t start, uint8_t end) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    uint8_t cmd[3];
    cmd[0] = BOOSTER_SETRANGE;
    cmd[1] = start;
    cmd[2] = end;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::setRainbow(uint16_t h, uint8_t s, uint8_t v, uint8_t start, uint8_t end, uint8_t step) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    if (h > 359) {
        h = 359;
    }
    uint8_t cmd[8];
    cmd[0] = BOOSTER_SETRAINBOW;
    cmd[1] = h & 0xFF;
    cmd[2] = h >> 8;
    cmd[3] = s;
    cmd[4] = v;
    cmd[5] = start;
    cmd[6] = end;
    cmd[7] = step;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::setGradient(int start, int end, uint8_t from[3], uint8_t to[3]) {
    if (start > end || start > _lastIndex) {
        return;
    }

    uint8_t steps = end - start;
    if (steps == 0) {
        setRGB(from[0], from[1], from[2]);
        return;
    }

    uint8_t s = 0, e = steps;
    if (start < 0) {
        s = 0 - start;
    }
    if (end > _lastIndex) {
        e -= (end - _lastIndex);
    }

    // optimized setRGB(r,g,b) and setLED(start + i) with one transaction and shared memory
    uint8_t cmd[6];
    cmd[0] = BOOSTER_SETRGB;
    cmd[4] = BOOSTER_SETLED;
    for (; s <= e; s++) {
        cmd[1] = from[0] + (to[0] - from[0]) * s / steps;
        cmd[2] = from[1] + (to[1] - from[1]) * s / steps;
        cmd[3] = from[2] + (to[2] - from[2]) * s / steps;
        cmd[5] = start + s;
        wiringPiSPIDataRW(0, cmd, sizeof (cmd));
    }
}

void DDBooster::shiftUp(uint8_t start, uint8_t end, uint8_t count) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    uint8_t cmd[4];
    cmd[0] = BOOSTER_SHIFTUP;
    cmd[1] = start;
    cmd[2] = end;
    cmd[3] = count;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::shiftDown(uint8_t start, uint8_t end, uint8_t count) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    uint8_t cmd[4];
    cmd[0] = BOOSTER_SHIFTDOWN;
    cmd[1] = start;
    cmd[2] = end;
    cmd[3] = count;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::copyLED(uint8_t from, uint8_t to) {
    if (from > _lastIndex || to > _lastIndex) {
        return;
    }
    uint8_t cmd[3];
    cmd[0] = BOOSTER_COPYLED;
    cmd[1] = from;
    cmd[2] = to;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::repeat(uint8_t start, uint8_t end, uint8_t count) {
    if (start > end || end > _lastIndex || start > _lastIndex) {
        return;
    }
    uint8_t cmd[4];
    cmd[0] = BOOSTER_REPEAT;
    cmd[1] = start;
    cmd[2] = end;
    cmd[3] = count;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
}

void DDBooster::show() {
    uint8_t cmd[1];
    cmd[0] = BOOSTER_SHOW;
    wiringPiSPIDataRW(0, cmd, sizeof (cmd));
    delayMicroseconds(BOOSTER_LED_DELAY * (_lastIndex + 1));
}
