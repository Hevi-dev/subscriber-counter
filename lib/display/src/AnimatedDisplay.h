#pragma once

#include <WString.h>
#include "Animation.h"
#include "Display.h"
#include "NullDisplay.h"

class AnimatedDisplay : public Display
{
public:
    AnimatedDisplay(Display *baseDisplay);
    ~AnimatedDisplay();

    void show(String text);
    void show(Animation *animation);
    void stop();
    void clear();
    uint8_t size();
    void tick();

private:
    String buffer;
    Display *display;
    Animation *currentAnimation = NULL;
};