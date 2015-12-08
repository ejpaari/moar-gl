#include "input.h"

namespace moar
{

Input::Input()
{
}

void Input::setCursorPosition(double x, double y)
{
    deltaX = x - this->x;
    deltaY = y - this->y;
    this->x = x;
    this->y = y;
}

void Input::setSensitivity(double sensitivity)
{
    this->sensitivity = sensitivity;
}

void Input::setMovementSpeed(float speed)
{
    movementSpeed = speed;
}

double Input::getCursorDeltaX() const
{
    return sensitivity * deltaX;
}

double Input::getCursorDeltaY() const
{
    return sensitivity * deltaY;
}

float Input::getMovementSpeed() const
{
    return movementSpeed;
}

} // moar
