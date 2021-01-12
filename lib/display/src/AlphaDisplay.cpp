#include <AlphaDisplay.h>

const uint8_t DisplaySize = 12;

AlphaDisplay::AlphaDisplay(uint8_t id0, uint8_t id1, uint8_t id2)
{
    displays[0].begin(id0);
    displays[1].begin(id1);
    displays[2].begin(id2);
}

void AlphaDisplay::show(String text)
{
    for (int i = 0; i < size(); ++i)
    {
        char c = i < text.length() ? text[i] : ' ';
        bool dot = c & 0x80;
        displays[i / 4].writeDigitAscii(i % 4, c & 0x7f, dot);
    }

    update();
}

void AlphaDisplay::clear()
{
    all([](LedDisplay *d, uint8_t i) { d->clear(); });
}

uint8_t AlphaDisplay::size()
{
    return DisplaySize;
}

void AlphaDisplay::update()
{
    all([](LedDisplay *d, uint8_t i) { d->writeDisplay(); });
}

void AlphaDisplay::all(void f(LedDisplay *g, uint8_t i))
{
    for (int i = 0; i < 3; ++i)
    {
        f(displays + i, i);
    }
}