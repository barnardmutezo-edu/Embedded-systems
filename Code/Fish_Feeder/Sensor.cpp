#include "Sensor.h"

Sensor::Sensor(const uint8_t && pinNumber){
    m_pinNumber = pinNumber;
    pinMode(m_pinNumber, INPUT);
}

uint16_t Sensor::reading(){

    return (uint16_t)map(analogRead(m_pinNumber), 0, 4095, 0, 255);  // Returns the analog value mapped between 0 and 255 from 4095 default value for esp32
}
