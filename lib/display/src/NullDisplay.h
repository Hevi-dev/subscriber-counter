#pragma once

#include <Display.h>

class NullDisplay : public Display
{
    void show(String text);
    void clear();
    uint8_t size();
};
