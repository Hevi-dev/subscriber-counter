#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <StatusLed.h>

void initializeWiFi(StatusLed &status, String ssid, String password)
{
  status.setColor(color_t(0, 0, 255));

  ssid.trim();
  password.trim();

  ssid = "The Fish Tank";
  password = "nowconnectingtotheworld";
  if (ssid.length() == 0)
  {
    status.setColor(color_t(255, 0, 0));
    Serial.println(F("Wifi SSID not set."));
    while (true)
    {
      delay(1000);
    }
  }

  WiFi.begin(ssid.c_str(), password.c_str());

  float brightness = 0.0f;
  while (WiFi.status() != WL_CONNECTED)
  {
    status.setColor(color_t(192, 255, 0), brightness);
    brightness += 0.01f;
    if (brightness > 1.0)
    {
      brightness = 0.0;
    }
    delay(10);
  }

  status.setColor(color_t(0, 255, 0));
  delay(250);

  status.clearColor();
}