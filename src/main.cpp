#include <Arduino.h>
#include <StatusLed.h>
#include <AlphaDisplay.h>
#include <AnimatedDisplay.h>
#include <animation/SweepAnimation.h>
#include <WiFi.h>

#include <SPIFFS.h>
#include <Effortless_SPIFFS.h>

#define STATUSLED_DATA 2
#define STATUSLED_CLOCK 12
StatusLed<STATUSLED_DATA, STATUSLED_CLOCK> status;
AnimatedDisplay *display;

const String defaultSplash = "*** HEVI ***";

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

String targets[] = {"Subscribers", "000000248791", "Views & Subs", "153M <> 248K"};
Timeout messageTimeout;

void setup()
{
  targets[3][0] |= 0x80; // add a decimal point to sample data

  Serial.begin(115200);
  display = new AnimatedDisplay(new AlphaDisplay());

  eSPIFFS fs(&Serial);

  String splash;
  if (!fs.openFromFile("/splash", splash))
  {
    splash = defaultSplash;
  }
  display->show(splash);

  messageTimeout.prepare(5000);

  // initializeWiFi(fs);
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
