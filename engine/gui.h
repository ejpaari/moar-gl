#ifndef GUI_H
#define GUI_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace moar
{

class GUI
{
public:
    GUI();
    GUI(const GUI&) = delete;
    GUI(GUI&&) = delete;
    GUI& operator=(const GUI&) = delete;
    GUI& operator=(GUI&&) = delete;

    bool init(GLFWwindow* window);
    void render();
    void uninit();
};

} // moar

#endif // GUI_H
