
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

#include <Animation.h>

class SweepAnimation : public Animation
{
public:
    SweepAnimation(String target, uint8_t charFrameDelay = 5, uint8_t frameLength = 15)
    {
        this->target = target;
        this->charFrameDelay = charFrameDelay;
        frameTimer.prepare(frameLength);
    }

    bool isRunning()
    {
        return inProgress;
    }

    bool tick(String &output)
    {
        if (frameTimer.periodic())
        {
            calculateNextFrame(output);
            return inProgress;
        }

        return false;
    }

    String stop()
    {
        inProgress = false;
        return target;
    }

private:
    void calculateNextFrame(String &buffer)
    {
        int length = target.length();
        inProgress = buffer.substring(0, length) != target;
        if (inProgress)
        {
            if (nextChar < length && frameCount % charFrameDelay == 0)
            {
                if (buffer.length() <= nextChar)
                {
                    buffer += " ";
                }
                char newChar = (nextChar >= length || target[nextChar] == ' ') ? ' ' : 'z';
                buffer[nextChar++] = newChar;
            }

            for (int i = 0; i < length; ++i)
            {
                char dot = target[i] & 0x80;
                char digit = buffer[i] & 0x7f;
                char targetDigit = target[i] & 0x7f;
                if (digit != targetDigit)
                {
                    if (i < nextChar && targetDigit != ' ')
                    {
                        buffer[i] = (digit - 1);
                    }
                }
                else
                {
                    buffer[i] = buffer[i] | dot;
                }
            }

            ++frameCount;
        }
    }

private:
    uint8_t nextChar = 0;
    uint32_t frameCount = 0;
    bool inProgress = true;
    Timeout frameTimer;

    String target;
    uint8_t charFrameDelay;
};