#ifndef ENGINE
#define ENGINE

#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace moar {

class Engine {
public:
    explicit Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) &  = delete;
    Engine& operator=(Engine&&) & = delete;

    bool initialize();
    void setApplication(Application* application) { app = application; }
    void start();

private:
    Application* app;
    GLFWwindow* window;
};

} // moar

#endif // ENGINE

