#include "WiFiManager.h"

void WiFiManager::setup(const char *ssid, const char *pass)
{

    WiFi.begin(ssid, pass);
}

bool WiFiManager::connected()
{
    return WiFi.status() == WL_CONNECTED;
}

DynamicJsonDocument WiFiManager::getJSON(const String &url)
{
    DynamicJsonDocument doc(1024); // Gotta make the buffer big enough..

    client.begin(url);
    uint8_t httpCode = client.GET();

    Serial.println("HTTP_CODE: ");
    Serial.println(httpCode);

    while (httpCode < 0)
    {
    Serial.println("Error on HTTP request");

        httpCode = client.GET(); 
        delay(500);
    }

    if (httpCode == 200)
    {
        Serial.println("FOUND IT!");

        Serial.println(httpCode);

        payload = client.getString();

        setJSON(doc, payload);

    }

    client.end();

    return doc;
}

void WiFiManager::setJSON(DynamicJsonDocument &doc, const String &payload)
{

    auto error = deserializeJson(doc, payload); // Checking error

    while (error)
    {
        Serial.println("ERRORRR");
        Serial.println(error.c_str());
        delay(200);
    }

    Serial.println("Got the json!");
}

String WiFiManager::getIP()
{
    return WiFi.dnsIP().toString();
}
