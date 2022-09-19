#include "MyServo.h"

MyServo::MyServo(const uint8_t &pinNumber){
      m_pinNumber = pinNumber;

}

void MyServo::operate()
{
  m_servo.attach(m_pinNumber, 500, 2000);

  m_servo.write(45);

  m_servo.write(0);
  delay(100);
  m_servo.write(45);
  delay(100);

  m_servo.write(0);
  delay(100);
  m_servo.write(45);
  delay(150);
  m_servo.detach();
    
}