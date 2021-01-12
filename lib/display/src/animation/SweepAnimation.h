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
            return true;
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
            if (nextChar < 12 && frameCount % charFrameDelay == 0)
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