#ifndef ENGINE
#define ENGINE

#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

namespace moar
{

class Engine
{
public:
    explicit Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

    bool init(const std::string& settingsFile);
    void setApplication(std::shared_ptr<Application> application) { app = application; }
    void execute();
    void input(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    void printInfo(int screenWidth, int screenHeight);

    std::shared_ptr<Application> app;
    GLFWwindow* window;
    bool useTimeLimit;
    double timeLimit;
};

} // moar

#endif // ENGINE

