#ifndef WiFiManager_H
#define WiFiManager_H

#include "Arduino.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"

class WiFiManager
{
public:
    explicit WiFiManager() = default;
    void setup(const char *ssid, const char *pass); // Sets up the wifi with credentials

    bool connected(); // returns wether or not the wifi is connected

    DynamicJsonDocument getJSON(const String &url); // returns the json data 

    String getIP(); // Return the IP needed for the weather API

private:
    HTTPClient client;
    String payload;
    void setJSON( DynamicJsonDocument &doc,const String &payload);
};
#endif // WiFiManager_H
