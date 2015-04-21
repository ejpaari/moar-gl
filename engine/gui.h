#ifndef GUI_H
#define GUI_H

#include <AntTweakBar.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

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

    bool init(int width, int height);
    void handleInput(GLFWwindow* window, int x, int y);
    void render();
    void uninit();

    TwBar* createBar(const std::string& name);
    TwBar* getBar(const std::string& name);

private:
    // Todo: Support multiple bars.
    TwBar* bar;
    double speed;
    bool wire;
    double time;
    glm::vec3 bgColor;
    unsigned int cubeColor;
};

} // moar

#endif // GUI_H
