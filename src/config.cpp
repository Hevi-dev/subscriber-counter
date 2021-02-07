#include "config.h"
#include <ArduinoJson.h>

void loadSettings(eSPIFFS &fs, config_t &config)
{
  String data;
  if (!fs.openFromFile("/secrets.json", data)) {
    Serial.println(F("No settings found to load"));
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

  config.secrets.ssid = doc["ssid"].as<String>();
  config.secrets.password = doc["password"].as<String>();
  config.secrets.yt_api_key = doc["yt_api_key"].as<String>();
  config.secrets.yt_channel_id = doc["yt_channel_id"].as<String>();
}