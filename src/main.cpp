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

#include <WiFi.h>
#include <SPIFFS.h>
#include <Effortless_SPIFFS.h>

#include <StatusLed.h>
#include <AlphaDisplay.h>
#include <AnimatedDisplay.h>
#include <animation/SweepAnimation.h>

#include <config.h>

#define STATUSLED_DATA 2
#define STATUSLED_CLOCK 12
StatusLed<STATUSLED_DATA, STATUSLED_CLOCK> status;
AnimatedDisplay *display;
config_t config;

const String defaultSplash = "*** HEVI ***";

void initializeWiFi(String ssid, String password)
{
  status.setColor(rgb_color(0, 0, 255));

  ssid.trim();
  password.trim();

  if (ssid.length() == 0)
  {
    status.setColor(rgb_color(255, 0, 0));
    Serial.println(F("Wifi SSID not set."));
    while (true)
    {
      sleep(600);
    }
  }

  WiFi.begin(ssid.c_str(), password.c_str());

  float brightness = 0.0f;
  while (WiFi.status() != WL_CONNECTED)
  {
    status.setColor(rgb_color(192, 255, 0), brightness);
    brightness += 0.01f;
    if (brightness > 1.0)
    {
      brightness = 0.0;
    }
    delay(10);
  }

  status.setColor(rgb_color(0, 255, 0));
  delay(250);

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  status.clearColor();
}

void showSplashScreen(eSPIFFS &fs)
{
  String splash;
  if (!fs.openFromFile("/splash", splash))
  {
    splash = defaultSplash;
  }
  display->show(splash);
}

String targets[] = {"Subscribers", "000000248791", "Views & Subs", "153M <> 248K"};
Timeout messageTimeout;

void setup()
{
  targets[3][0] |= 0x80; // add a decimal point to sample data

  Serial.begin(115200);
  display = new AnimatedDisplay(new AlphaDisplay());

  eSPIFFS fs(&Serial);

  showSplashScreen(fs);
  loadSettings(fs, config);
  initializeWiFi(config.secrets.ssid, config.secrets.password);

  messageTimeout.prepare(5000);
}

uint8_t currentTarget = 0;

void loop()
{
  if (messageTimeout.periodic())
  {
    display->show(new SweepAnimation(targets[currentTarget]));
    currentTarget = (currentTarget + 1) % 4;
  }

  display->tick();
}
