#pragma once

#include "Display.h"
#include "Adafruit_LEDBackpack.h"

#define LedDisplay Adafruit_AlphaNum4

class AlphaDisplay : public Display
{
public:
    AlphaDisplay(uint8_t id0 = 0x70, uint8_t id1 = 0x72, uint8_t id2 = 0x74);

    void show(String text);
    void clear();
    uint8_t size();

private:
    LedDisplay displays[3];

    void update();
    void all(void f(LedDisplay *g, uint8_t i));
};