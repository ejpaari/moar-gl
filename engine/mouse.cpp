#include "mouse.h"

namespace moar
{

Mouse::Mouse() :
    x(0.0),
    y(0.0),
    deltaX(0.0),
    deltaY(0.0),
    sensitivity(1.0)
{
}

Mouse::~Mouse()
{
}

void Mouse::setPosition(double x, double y)
{
    deltaX = x - this->x;
    deltaY = y - this->y;
    this->x = x;
    this->y = y;
}

double Mouse::getDeltaX() const
{
    return sensitivity * (deltaX * 0.01);
}

double Mouse::getDeltaY() const
{
    return sensitivity * (deltaY * 0.01);
}

} // moar
