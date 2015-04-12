#ifndef APPLICATION
#define APPLICATION

#include "input.h"
#include "object.h"
#include "renderer.h"
#include "material.h"
#include "camera.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <vector>
#include <memory>

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

    void setEngine(Engine* engine) { this->engine = engine; }

    virtual void start() = 0;
    virtual void handleInput(GLFWwindow* window);
    virtual void update(double time) = 0;
    void render();
    void quit() { running = false; }

    Object* createRenderObject(const std::string& shaderName, const std::string& modelName, const std::string& textureName = "");

    bool isRunning() const { return running; }

    Engine* getEngine() { return engine; }
    Input* getInput() { return &input; }

private:
    Engine* engine;
    Camera camera;
    Input input;
    bool running;

    std::vector<std::shared_ptr<Object>> renderObjects;
};

} // moar

#endif // APPLICATION
