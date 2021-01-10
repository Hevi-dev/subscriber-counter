#include <Arduino.h>
#include <StatusLed.h>
#include <Display.h>
#include <WiFi.h>

#include <SPIFFS.h>
#include <Effortless_SPIFFS.h>

#define STATUSLED_DATA 2
#define STATUSLED_CLOCK 12
StatusLed<STATUSLED_DATA, STATUSLED_CLOCK> status;
Display *display;

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

void setup()
{
  Serial.begin(115200);
  display = new Display();

  eSPIFFS fs(&Serial);

  String splash;
  if (!fs.openFromFile("/splash", splash))
  {
    splash = defaultSplash;
  }
  display->show(splash);

  initializeWiFi(fs);

  targets[3][0] |= 0x80; // add a decimal point to sample data
}

uint8_t currentTarget = 0;
String buffer = "            ";

void loop()
{
  String target = targets[currentTarget];
  currentTarget = (currentTarget + 1) % 4;

  uint8_t nextChar = 0;
  uint32_t frameCount = 0;
  uint8_t frameDelay = 5;
  uint8_t length = target.length();

  int start = millis();
  do
  {
    if (nextChar < display->size() && frameCount % frameDelay == 0)
    {
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
    display->show(buffer);
    delay(10);
  } while (buffer.substring(0, length) != target);

  int elapsed = millis() - start;
  char resultText[60];
  snprintf(resultText, sizeof(resultText), "Elapsed: %4d; Frames: %4d; FPS: %d", elapsed, frameCount, frameCount * 1000 / elapsed);
  Serial.println(resultText);

  delay(5000);
}
