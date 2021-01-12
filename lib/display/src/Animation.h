#pragma once

#include <WString.h>
#include <timeout.h>

class Animation
{
public:
    virtual ~Animation(){};
    virtual bool tick(String &output) = 0;
    virtual bool isRunning() = 0;
    virtual String stop() = 0;
};
