#ifndef Button_H
#define Button_H
#include <Arduino.h>

class Button
{
public:
    explicit Button(const uint8_t &&pinNumber);
    //void setup(const uint8_t &pinNumber);
    bool isPressed();
    bool stateChanged();

    void readState();
    void updateState();

private:
    uint8_t m_pinNumber;
    uint8_t currentState;
    uint8_t prevState;
};

#endif // Button_H
