#pragma once

#include "Adafruit_LEDBackpack.h"

#define LedDisplay Adafruit_AlphaNum4

class Display
{
public:
    Display(uint8_t id0 = 0x70, uint8_t id1 = 0x72, uint8_t id2 = 0x74)
    {
        displays[0].begin(id0);
        displays[1].begin(id1);
        displays[2].begin(id2);
    }

    void show(String text)
    {
        for (int i = 0; i < 12; ++i)
        {
            char c = i < text.length() ? text[i] : ' ';
            bool dot = c & 0x80;
            displays[i / 4].writeDigitAscii(i % 4, c & 0x7f, dot);
        }

        update();
    }

    void clear()
    {
        all([](LedDisplay *d, uint8_t i) { d->clear(); });
    }

    uint8_t size()
    {
        return 12;
    }

private:
    LedDisplay displays[3];

    void update()
    {
        all([](LedDisplay *d, uint8_t i) { d->writeDisplay(); });
    }

    void all(void f(LedDisplay *g, uint8_t i))
    {
        for (int i = 0; i < 3; ++i)
        {
            f(displays + i, i);
        }
    }
};