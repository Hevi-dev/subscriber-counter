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

#pragma once

#include "Display.h"

typedef uint8_t page_index_t;

class DisplaySource
{
public:
    virtual void onPageUpdated(page_index_t page) = 0;
    virtual uint8_t size() = 0;
};

template <uint8_t pageCount>
class PagedDisplay : private DisplaySource
{
public:
    PagedDisplay(Display *display) : baseDisplay(display)
    {
        for (int t = 0; t < pageCount; ++t)
        {
            pages[t].initialize(this, t);
        }
    }

    Display *getDisplayForPage(page_index_t page)
    {
        verifyPageNumber(page);
        return &pages[page];
    }

    page_index_t nextPage()
    {
        gotoPage((currentPage + 1) % pageCount);
        return currentPage;
    }

    page_index_t previousPage()
    {
        gotoPage((currentPage == 0 ? pageCount : currentPage) - 1);
        return currentPage;
    }

    page_index_t gotoPage(page_index_t page)
    {
        verifyPageNumber(page);
        auto previousPage = currentPage;
        currentPage = page;
        updateDisplay();
        return previousPage;
    }

    page_index_t getCurrentPage()
    {
        return currentPage;
    }

private:
    inline static void verifyPageNumber(page_index_t page)
    {
        assert(page < pageCount);
    }

    void updateDisplay()
    {
        baseDisplay->show(pages[currentPage].getBuffer());
    }

    uint8_t size()
    {
        return baseDisplay->size();
    }

    void onPageUpdated(page_index_t page)
    {
        if (page == currentPage)
        {
            updateDisplay();
        }
    }

private:
    class Page : public Display
    {
    public:
        void initialize(DisplaySource *parent, page_index_t pageIndex)
        {
            verifyPageNumber(pageIndex);
            this->parent = parent;
            this->pageIndex = pageIndex;
        }

        void show(String text)
        {
            buffer = text;
            parent->onPageUpdated(pageIndex);
        }

        void clear()
        {
            show("");
        }

        uint8_t size()
        {
            return parent->size();
        }

        String getBuffer()
        {
            return buffer;
        }

    private:
        DisplaySource *parent;

        String buffer;
        page_index_t pageIndex;
    };

private:
    Page pages[pageCount];
    page_index_t currentPage = 0;
    Display *baseDisplay;
};
