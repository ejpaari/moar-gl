#ifndef ENGINE
#define ENGINE

#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <utility>

namespace moar {

class Engine {
public:
    explicit Engine(const std::string& settingsFile);
    ~Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) &  = delete;
    Engine& operator=(Engine&&) & = delete;

    bool initialize();
    void setApplication(Application* application) { app = application; }
    void execute();

private:
    Application* app;
    GLFWwindow* window;
    std::string settingsFile;
    std::pair<bool, double> timeLimit;
};

} // moar

#endif // ENGINE

