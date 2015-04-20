#include "gui.h"

namespace moar
{

GUI::GUI()
{
}

GUI::~GUI()
{    
}

bool GUI::init(int width, int height)
{
    int ok = TwInit(TW_OPENGL_CORE, NULL);
    if (ok == 0) {
        return false;
    }

    bar = TwNewBar("TweakBar");
    TwAddVarRW(bar, "speed", TW_TYPE_DOUBLE, &speed, "");
    TwAddVarRW(bar, "wire", TW_TYPE_BOOL32, &wire, "");
    TwAddVarRO(bar, "time", TW_TYPE_DOUBLE, &time, "");
    TwAddVarRW(bar, "bgColor", TW_TYPE_COLOR3F, &bgColor, "");
    TwAddVarRW(bar, "cubeColor", TW_TYPE_COLOR32, &cubeColor, "");
    TwWindowSize(width, height);

    return true;
}

void GUI::handleInput(GLFWwindow* window, int x, int y)
{
    TwMouseMotion(x, y);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
    }
}

} // moar
