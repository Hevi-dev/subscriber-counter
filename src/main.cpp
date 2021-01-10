#include <Arduino.h>
#include <StatusLed.h>
#include <WiFi.h>

#include <SPIFFS.h>
#include <Effortless_SPIFFS.h>

#define STATUSLED_DATA 2
#define STATUSLED_CLOCK 12
StatusLed<STATUSLED_DATA, STATUSLED_CLOCK> status;

void initializeWiFi(eSPIFFS &fs)
{
  status.setColor(rgb_color(0, 0, 255));
  String ssid;
  String password;

  fs.openFromFile("/wifi-ssid", ssid);
  fs.openFromFile("/wifi-password", password);

  ssid.trim();
  password.trim();

  if (ssid.length() == 0)
  {
    status.setColor(rgb_color(255, 0, 0));
    Serial.println("Wifi SSID not set.");
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

void setup()
{
  Serial.begin(115200);
  delay(1000);
  
  eSPIFFS fs(&Serial);

  initializeWiFi(fs);
}

void loop()
{
}
