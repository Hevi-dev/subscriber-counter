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