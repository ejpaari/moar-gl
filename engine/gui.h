#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>

namespace moar
{

class GUI
{
public:
    GUI();
    ~GUI();
    GUI(const GUI&) = delete;
    GUI(GUI&&) = delete;
    GUI& operator=(const GUI&) = delete;
    GUI& operator=(GUI&&) = delete;

    bool init(GLFWwindow* window);
    void uninit();
    void render();
    void keyCallback(int key, int action);
};

} // moar

#endif // GUI_H
