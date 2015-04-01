#ifndef APPLICATION
#define APPLICATION

#include "renderobject.h"
#include "shader.h"
#include "camera.h"
#include "mouse.h"

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
    virtual void input(GLFWwindow* window);
    virtual void update(double time) = 0;
    void render();
    void quit() { running = false; }

    RenderObject* createRenderObject(const std::string& shaderName, const std::string& modelName);

    bool isRunning() const { return running; }

private:
    Camera camera;
    Mouse mouse;
    bool running;
    float movementSpeed;

    std::vector<std::shared_ptr<RenderObject>> renderObjects;
    std::map<std::string, std::unique_ptr<Shader>> shaders;
};

} // moar

#endif // APPLICATION
