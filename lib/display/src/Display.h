#pragma once

#include <WString.h>
#include "Animation.h"

class Display
{
public:
    virtual ~Display() {}
    
    virtual void show(String text) = 0;
    virtual void clear() = 0;
    virtual uint8_t size() = 0;
};
