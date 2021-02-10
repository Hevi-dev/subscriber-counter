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

    stats_t getChannelStatistics(String channelId, String apiKey)
    {
        stats_t result = {0};
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
            auto statisticsFilter = filter["items"][0]["statistics"];
            statisticsFilter["subscriberCount"] = true;
            statisticsFilter["viewCount"] = true;

            DynamicJsonDocument doc(256 + length);
            ReadBufferingClient buffer(http, length);
            auto error = deserializeJson(doc, buffer, DeserializationOption::Filter(filter));

            if (!error)
            {
                auto statistics = doc["items"][0]["statistics"];
                result.subscriberCount = statistics["subscriberCount"];
                result.viewCount = statistics["viewCount"];
                result.valid = true;
            }
            else
            {
                Serial.printf("Error parsing document: %s\n", error.c_str());
                Serial.println(http.responseBody());
            }
        }
        else
        {
            Serial.printf("Error loading document: %d\n", statusCode);
            Serial.println(http.responseBody());
        }

        return result;
    }
} // namespace youtube