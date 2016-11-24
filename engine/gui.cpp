#include "gui.h"

#include <AntTweakBar.h>

namespace
{

void mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
{
    TwEventMouseButtonGLFW(button, action);
}

void cursorPosCallback(GLFWwindow* /*window*/, double x, double y)
{
    TwEventMousePosGLFW(static_cast<int>(x), static_cast<int>(y));
}

void charCallback(GLFWwindow* /*window*/, unsigned int codepoint)
{
    TwEventCharGLFW(codepoint, GLFW_PRESS);
}

} // anonymous

namespace moar
{

GUI::GUI()
{
}

GUI::~GUI()
{
}

bool GUI::init(GLFWwindow* window)
{
    int ok = TwInit(TW_OPENGL_CORE, NULL);
    if (ok == 0) {
        return false;
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    TwWindowSize(width, height);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetCharCallback(window, charCallback);
    return true;
}

void GUI::uninit()
{
    TwTerminate();
}

void GUI::render()
{
    TwDraw();
}

void GUI::keyCallback(int key, int action)
{
    TwEventKeyGLFW(key, action);
}

} // moar
