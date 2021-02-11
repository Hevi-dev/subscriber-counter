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

#include <SPIFFS.h>
#include <Effortless_SPIFFS.h>

#include <DotStarStatusLed.h>
#include <PagedDisplay.h>
#include <AlphaDisplay.h>
#include <AnimatedDisplay.h>
#include <animation/SweepAnimation.h>

#include <config.h>
#include <wifi_setup.h>
#include <youtube.h>

#define STATUSLED_DATA 2
#define STATUSLED_CLOCK 12
DotStarStatusLed<STATUSLED_DATA, STATUSLED_CLOCK> status;

PagedDisplay<2> *pagedDisplay;
Display *display;
AnimatedDisplay *subDisplay, *viewDisplay;
Timeout fetchDataTimeout;
Timeout pageTimeout;
settings_t config;

#define SECONDS (1000)
#define MINUTES (SECONDS * 60)

void setup()
{
  Serial.begin(115200);
  pagedDisplay = new PagedDisplay<2>(new AlphaDisplay());
  display = pagedDisplay->getDisplayForPage(0);
  subDisplay = new AnimatedDisplay(display);
  viewDisplay = new AnimatedDisplay(pagedDisplay->getDisplayForPage(1));

  eSPIFFS fs(&Serial);

  loadConfiguration(fs, config);

  display->show(config.splashScreen);

  initializeWiFi(status, config.secrets.ssid, config.secrets.password);

  fetchDataTimeout.prepare(config.youtubeRefreshMinutes * MINUTES);
  pageTimeout.start(10 * SECONDS);
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
        snprintf(text, sizeof(text), "V %010d", stats.viewCount);
        viewDisplay->show(new SweepAnimation(text));
      }
      if (stats.subscriberCount != oldSubCount)
      {
        oldSubCount = stats.subscriberCount;
        char text[13];
        snprintf(text, sizeof(text), "S %010d", stats.subscriberCount);
        subDisplay->show(new SweepAnimation(text));
      }
    }
  }

  if (pageTimeout.periodic()) {
    pagedDisplay->nextPage();
  }

  subDisplay->tick();
  viewDisplay->tick();
}
