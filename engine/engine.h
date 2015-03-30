#ifndef ENGINE
#define ENGINE

#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <functional>

namespace moar
{

class Engine
{
public:
    explicit Engine(const std::string& settingsFile);
    ~Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

    bool init();
    void setApplication(std::shared_ptr<Application> application) { app = application; }
    void execute();
    void input(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    std::shared_ptr<Application> app;
    GLFWwindow* window;
    std::string settingsFile;
    bool useTimeLimit;
    double timeLimit;
};

} // moar

#endif // ENGINE

