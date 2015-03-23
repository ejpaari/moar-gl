#ifndef ENGINE
#define ENGINE

#include "application.h"

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

}

#endif // ENGINE

