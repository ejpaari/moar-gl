#include "gui.h"

#include <AntTweakBar.h>

namespace
{
#ifdef ANT
void mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
{
    TwEventMouseButtonGLFW(button, action);
}

void cursorPosCallback(GLFWwindow* /*window*/, double x, double y)
{
    TwEventMousePosGLFW(static_cast<int>(x), static_cast<int>(y));
}

void keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    TwEventKeyGLFW(key, action);
}

void charCallback(GLFWwindow* /*window*/, unsigned int codepoint)
{
    TwEventCharGLFW(codepoint, GLFW_PRESS);
}
#endif
} // anonymous

namespace moar
{

GUI::GUI()
{
}

bool GUI::init(GLFWwindow* /*window*/)
{
#ifdef ANT
    int ok = TwInit(TW_OPENGL_CORE, NULL);
    if (ok == 0) {
        return false;
    }

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    TwWindowSize(width, height);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, charCallback);
#endif
    return true;
}

void GUI::render()
{
#ifdef ANT
    TwDraw();
#endif
}

void GUI::uninit()
{
#ifdef ANT
    TwTerminate();
#endif
}

} // moar
