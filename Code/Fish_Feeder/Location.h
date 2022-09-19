#ifndef Location_H
#define Location_H
#include "Arduino.h"

// Simple struct that holds geolocation data
struct Location
{
    explicit Location() = default;
    void setup(const String & countryCode,const String & city , const float &lat, const float &lon);

    String m_countryCode;
    String m_city;
    float m_lat;
    float m_lon;

};
#endif // Location_H
