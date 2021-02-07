#pragma once

#include <Arduino.h>
#include <Effortless_SPIFFS.h>

struct secrets_t
{
  String ssid;
  String password;
  String yt_api_key;
  String yt_channel_id;
};
// NOTE: Keep this number in sync with the above struct.
#define CONFIG_NUMBER_OF_SECRETS 4

struct config_t {
    secrets_t secrets;
};

void loadSettings(eSPIFFS &fs, config_t &config);