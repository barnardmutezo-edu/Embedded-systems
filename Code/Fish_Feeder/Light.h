#ifndef Light_H
#define Light_H
#include "Arduino.h"

class Light
{
public:
    explicit Light(const uint8_t &&pinNumber);
    void turnOn();
    void turnOff();

private:
    uint8_t m_pinNumber;
};
#endif // Light_H
