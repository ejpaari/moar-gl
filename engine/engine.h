#ifndef ENGINE
#define ENGINE

#include "application.h"
#include "resourcemanager.h"
#include "gui.h"

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

    void setApplication(std::shared_ptr<Application> application);
    bool init(const std::string& settingsFile);
    void execute();

    ResourceManager* getResourceManager() { return &manager; }

private:
    void printInfo(int windowWidth, int windowHeight);

    std::shared_ptr<Application> app;
    ResourceManager manager;
    GUI gui;
    GLFWwindow* window;
    bool useTimeLimit;
    double timeLimit;
};

} // moar

#endif // ENGINE

