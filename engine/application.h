#ifndef APPLICATION
#define APPLICATION

#include "renderobject.h"
#include "shader.h"
#include "camera.h"
#include "input.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <vector>
#include <map>
#include <memory>

namespace moar
{

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
    virtual void handleInput(GLFWwindow* window);
    virtual void update(double time) = 0;
    void render();
    void quit() { running = false; }

    RenderObject* createRenderObject(const std::string& shaderName, const std::string& modelName);

    bool isRunning() const { return running; }

    Input* getInput() { return &input; }

private:
    Camera camera;
    Input input;
    bool running;

    std::vector<std::shared_ptr<RenderObject>> renderObjects;
    std::map<std::string, std::unique_ptr<Shader>> shaders;
};

} // moar

#endif // APPLICATION
