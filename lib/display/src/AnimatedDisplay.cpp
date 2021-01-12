#include "AnimatedDisplay.h"

AnimatedDisplay::AnimatedDisplay(Display *baseDisplay)
{
    display = baseDisplay;
    if (display == NULL)
    {
        display = new NullDisplay();
    }
}

AnimatedDisplay::~AnimatedDisplay()
{
    delete display;
}

void AnimatedDisplay::show(String text)
{
    stop();
    display->show(text);
}

void AnimatedDisplay::show(Animation *animation)
{
    Animation *oldAnimation = currentAnimation;
    currentAnimation = animation;
    if (oldAnimation)
    {
        delete oldAnimation;
    }
}

void AnimatedDisplay::stop()
{
    if (currentAnimation && currentAnimation->isRunning())
    {
        display->show(currentAnimation->stop());
    }

    delete currentAnimation;
    currentAnimation = NULL;
}

void AnimatedDisplay::clear()
{
    stop();
    display->clear();
}

uint8_t AnimatedDisplay::size()
{
    return display->size();
}

void AnimatedDisplay::tick()
{
    String output = buffer;
    if (currentAnimation && currentAnimation->tick(output))
    {
        buffer = output;
        display->show(buffer);
    }
}
