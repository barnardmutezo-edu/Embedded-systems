#include "Light.h"

Light::Light(const uint8_t &&pinNumber)
{

    m_pinNumber = pinNumber;
    pinMode(m_pinNumber, OUTPUT);
    turnOff(); // prevents the light from booting on
}

void Light::turnOn()
{
    Serial.println("Lamp is on!");
    digitalWrite(m_pinNumber, LOW);
}

void Light::turnOff()
{
    Serial.println("Lamp is off!");

    digitalWrite(m_pinNumber, HIGH);
}