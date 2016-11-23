#ifndef APPLICATION_H
#define APPLICATION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <AntTweakBar.h>

namespace moar
{

class Engine;

class Application
{
public:
    explicit Application();
    virtual ~Application();
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    virtual void start() = 0;
    virtual void levelLoaded() = 0;
    virtual void handleInput(GLFWwindow* window) = 0;
    virtual void update() = 0;
    virtual void quit();

    void setEngine(Engine* engine);
    bool isRunning() const;

protected:
    Engine* engine = nullptr;

private:    
    bool running = true;
};

} // moar

#endif // APPLICATION_H
