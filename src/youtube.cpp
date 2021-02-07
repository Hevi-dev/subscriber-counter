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

#include "youtube.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HttpClient.h>
#include <StreamUtils.h>

namespace youtube
{
    const String channelEndpoint = "/youtube/v3/channels?part=statistics&id=<CHANNEL>&key=<APIKEY>";
    const String youtubeHost = "youtube.googleapis.com";

    String getSubscriberCount(String channelId, String apiKey)
    {
        String result = F("* No Data *");
        WiFiClientSecure client;
        HttpClient http(client, youtubeHost, 443);
        String endpoint = channelEndpoint;
        endpoint.replace("<CHANNEL>", channelId);
        endpoint.replace("<APIKEY>", apiKey);

        http.get(endpoint);
        auto statusCode = http.responseStatusCode();
        if (statusCode == 200)
        {
            auto length = http.contentLength();

            StaticJsonDocument<200> filter;
            filter["items"][0]["statistics"]["subscriberCount"] = true;

            DynamicJsonDocument doc(256 + length);
            ReadBufferingClient buffer(http, 128);
            deserializeJson(doc, buffer, DeserializationOption::Filter(filter));
            uint32_t count = doc["items"][0]["statistics"]["subscriberCount"];
            char text[13];
            snprintf(text, sizeof(text), "%012d", count);
            result = text;
        }
        else
        {
            Serial.printf("Error loading document: %d\n", statusCode);
            Serial.println(http.responseBody());
        }

        return result;
    }
} // namespace youtube