#ifndef Sensor_H
#define Sensor_H
#include "Arduino.h"

class Sensor
{
public:
    explicit Sensor(const uint8_t && pinNumber);

    uint16_t reading();

private:
    uint8_t m_pinNumber;
};
#endif // Sensor_H
