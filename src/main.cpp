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

#include <Arduino.h>

#include <EButton.h>

#include <PagedDisplay.h>
#include <AnimatedDisplay.h>
#include <animation/SweepAnimation.h>

#include <config.h>
#include <wifi_setup.h>
#include <youtube.h>

#ifdef DISPLAY_ALPHA
#include <AlphaDisplay.h>
#else
#include <NullDisplay.h>
#endif

#ifdef STATUS_DOTSTAR
#include <DotStarStatusLed.h>
DotStarStatusLed<STATUS_DOTSTAR_DATA, STATUS_DOTSTAR_CLOCK> status;
#elif STATUS_NEOPIXEL
#include <StatusLed.h>
NullStatusLed status;
#endif

#define BASE_PAGE_COUNT 2
#ifdef HAS_RTC
#define CLOCKPAGE_COUNT 1
Display *clockDisplay;
#else
#define CLOCKPAGE_COUNT 0
#endif
const int pageCount = BASE_PAGE_COUNT + CLOCKPAGE_COUNT;

Display *display;
PagedDisplay<pageCount> *pagedDisplay;
AnimatedDisplay *subDisplay, *viewDisplay;
Timeout fetchDataTimeout;
settings_t config;

#define DEBOUNCE_TIME 10
const int nextPagePin = 14;
const int previousPagePin = 15;
EButton nextPageButton(nextPagePin, false);
EButton previousPageButton(previousPagePin, false);
Timeout pageCycleTimeout;
bool cyclePages = false;

#define SECONDS (1000)
#define MINUTES (SECONDS * 60)

void nextPage(EButton &)
{
  pagedDisplay->nextPage();
}

void previousPage(EButton &)
{
  pagedDisplay->previousPage();
}

void togglePageCycle(EButton &btn)
{
  cyclePages = true;
  pageCycleTimeout.reset();
  nextPage(btn);
}

Timeout updateClockTimeout;

void updateClock()
{
#ifdef HAS_RTC
  tm currentTime;
  if (updateClockTimeout.periodic() && getLocalTime(&currentTime))
  {
    char timeText[13];
    snprintf(timeText, sizeof(timeText), "T  %2d %02d %02d", currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec);
    clockDisplay->show(timeText);
  }
#endif
}

void setup()
{
  Serial.begin(115200);
#ifdef DISPLAY_ALPHA
  auto hardwareDisplay = new AlphaDisplay();
#else
  auto hardwareDisplay = new NullDisplay();
#endif
  pagedDisplay = new PagedDisplay<pageCount>(hardwareDisplay);
  display = pagedDisplay->getDisplayForPage(0);
  subDisplay = new AnimatedDisplay(display);
  viewDisplay = new AnimatedDisplay(pagedDisplay->getDisplayForPage(1));
#ifdef HAS_RTC
  clockDisplay = pagedDisplay->getDisplayForPage(2);
#endif

  loadConfiguration(config);

  display->show(config.splashScreen);

  initializeWiFi(status, config.secrets.ssid, config.secrets.password);
  #ifdef ESP32
  configTime(config.utcOffsetMinutes * 60, 0, "pool.ntp.org", "time.nist.gov");
  #endif

  fetchDataTimeout.prepare(config.youtubeRefreshMinutes * MINUTES);
  pageCycleTimeout.start(10 * SECONDS);
  updateClockTimeout.prepare(100);

  nextPageButton.attachEachClick(nextPage);
  nextPageButton.setDebounceTime(DEBOUNCE_TIME);
  nextPageButton.attachLongPressStart(togglePageCycle);
  previousPageButton.attachEachClick(previousPage);
  previousPageButton.setDebounceTime(DEBOUNCE_TIME);
  previousPageButton.attachLongPressStart(togglePageCycle);
}

void loop()
{
  static uint32_t oldViewCount = UINT32_MAX;
  static uint32_t oldSubCount = UINT32_MAX;

  if (fetchDataTimeout.periodic())
  {
    status.setColor(color_t(255, 255, 255));
    youtube::stats_t stats = youtube::getChannelStatistics(config.secrets.yt_channel_id, config.secrets.yt_api_key);
    status.clearColor();
    if (!stats.valid)
    {
      display->show(F("* No Data *"));
    }
    else
    {
      if (stats.viewCount != oldViewCount)
      {
        oldViewCount = stats.viewCount;
        char text[13];
        snprintf(text, sizeof(text), "V %010u", (unsigned int)stats.viewCount);
        viewDisplay->show(new SweepAnimation(text));
      }
      if (stats.subscriberCount != oldSubCount)
      {
        oldSubCount = stats.subscriberCount;
        char text[13];
        snprintf(text, sizeof(text), "S %010u", (unsigned int)stats.subscriberCount);
        subDisplay->show(new SweepAnimation(text));
      }
    }
  }

  if (cyclePages && pageCycleTimeout.periodic())
  {
    pagedDisplay->nextPage();
  }

  updateClock();

  nextPageButton.tick();
  previousPageButton.tick();
  subDisplay->tick();
  viewDisplay->tick();
}
