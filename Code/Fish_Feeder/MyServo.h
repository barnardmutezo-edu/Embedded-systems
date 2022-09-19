#ifndef MyServo_H
#define MyServo_H
#include "Arduino.h"
#include "ESP32Servo.h"

class MyServo
{
public:
    explicit MyServo(const uint8_t &pinNumber);
    void operate();

private:
    uint8_t m_pinNumber;
    Servo m_servo;
};
#endif // MyServo_H
