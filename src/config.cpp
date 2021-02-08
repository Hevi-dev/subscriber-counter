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

#include "config.h"
#include <ArduinoJson.h>

#define SETTINGS_FILE "/settings.json"
#define SECRETS_FILE "/secrets.json"

void loadSecrets(eSPIFFS &fs, secrets_t &secrets) {
  String data;
  if (!fs.openFromFile(SECRETS_FILE, data)) {
    Serial.println(F("No secrets found to load"));
    return;
  }

  auto capacity = JSON_OBJECT_SIZE(CONFIG_NUMBER_OF_SECRETS) + data.length();
  DynamicJsonDocument doc(capacity);
  auto error = deserializeJson(doc, data);
  if (error)
  {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }

  secrets.ssid = doc["ssid"].as<String>();
  secrets.password = doc["password"].as<String>();
  secrets.yt_api_key = doc["yt_api_key"].as<String>();
  secrets.yt_channel_id = doc["yt_channel_id"].as<String>();
}

void loadSettings(eSPIFFS &fs, settings_t &config) {
  String data;
  if (!fs.openFromFile(SETTINGS_FILE, data)) {
    Serial.println(F("No settings found to load"));
    return;
  }

  auto capacity = JSON_OBJECT_SIZE(CONFIG_NUMBER_OF_SETTINGS) + data.length();
  DynamicJsonDocument doc(capacity);
  auto error = deserializeJson(doc, data);
  if (error)
  {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return;
  }

  config.youtubeRefreshMinutes = doc["yt_refresh_minutes"].as<uint16_t>();
}

void loadConfiguration(eSPIFFS &fs, settings_t &config)
{
    loadSettings(fs, config);
    loadSecrets(fs, config.secrets);
}