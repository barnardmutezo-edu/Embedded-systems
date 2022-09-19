#include "Location.h"

void Location::setup(const String & countryCode,const String & city ,const float &lat, const float &lon)
{
    m_countryCode = countryCode;
    m_city = city;
    m_lat = lat;
    m_lon = lon;
}


