#include "time.h"

#include <GLFW/glfw3.h>

namespace moar
{

Time::Time()
{
}

void Time::update()
{
    float t = static_cast<float>(glfwGetTime());
    delta = t - time;
    time = t;
}

float Time::getTime() const
{
    return time;
}

float Time::getDelta() const
{
    return delta;
}

} // moar
