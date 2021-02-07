//----------------------------------------------------------------------------//
//                                                                            //
// Copyright (c) 2021 Hevi Development, LLC                                   //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
//                                                                            //
//----------------------------------------------------------------------------//

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
    tick();
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
