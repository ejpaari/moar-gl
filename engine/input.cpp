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

bool Input::isKeyPressed(int key)
{
    return keys[key].pressed;
}

bool Input::isKeyDown(int key)
{
    return keys[key].down;
}

void Input::handleInput(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (action == GLFW_PRESS) {
        keys[key].down = true;
        keys[key].pressed = true;
    }
    if (action == GLFW_RELEASE) {
        keys[key].down = false;
    }    
}

void Input::reset()
{
    for (auto& kv : keys) {
        kv.second.pressed = false;
    }
}

} // moar
