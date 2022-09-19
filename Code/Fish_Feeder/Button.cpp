#include "Button.h"

Button::Button(const uint8_t &&pinNumber)
{
    m_pinNumber = pinNumber;
    prevState = 0;
    pinMode(m_pinNumber, INPUT);
    readState();
}

bool Button::isPressed()
{
    return stateChanged() && currentState;
}

bool Button::stateChanged()
{
    return currentState != prevState;
}

void Button::readState()
{
    currentState = digitalRead(m_pinNumber);
}

void Button::updateState()
{

    prevState = currentState;
}
