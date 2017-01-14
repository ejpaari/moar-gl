#include "input.h"

namespace moar
{

Input::Input()
{
}

Input::~Input()
{
}

void Input::setCursorPosition(float x, float y)
{
    deltaX = x - this->x;
    deltaY = y - this->y;
    this->x = x;
    this->y = y;
}

void Input::setCursorPosition(double x, double y)
{
	float fx = static_cast<float>(x);
	float fy = static_cast<float>(y);
	deltaX = fx - this->x;
	deltaY = fy - this->y;
	this->x = fx;
	this->y = fy;
}

void Input::setSensitivity(float sensitivity)
{
    this->sensitivity = sensitivity;
}

void Input::setMovementSpeed(float speed)
{
    movementSpeed = speed;
}

float Input::getCursorDeltaX() const
{
    return sensitivity * deltaX;
}

float Input::getCursorDeltaY() const
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

void Input::handleKey(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (action == GLFW_PRESS) {
        keys[key].down = true;
        keys[key].pressed = true;
    }
    if (action == GLFW_RELEASE) {
        keys[key].down = false;
    }
    if (gui) {
        gui->keyCallback(key, action);
    }
}

void Input::reset()
{
    for (auto& kv : keys) {
        kv.second.pressed = false;
    }
}

void Input::resetCursorDelta()
{
    deltaX = 0.0;
    deltaY = 0.0;
}

void Input::setGUI(GUI* gui)
{
    this->gui = gui;
}

} // moar
