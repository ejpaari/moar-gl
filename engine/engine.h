#ifndef ENGINE
#define ENGINE

#include "application.h"
#include "resourcemanager.h"
#include "gui.h"
#include "input.h"
#include "rendersettings.h"
#include "camera.h"
#include "object.h"
#include "framebuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>
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

    void setApplication(Application* application);
    bool init(const std::string& settingsFile);
    void execute();

    ResourceManager* getResourceManager();
    Camera* getCamera();
    Input* getInput();
    RenderSettings* getRenderSettings();

    void addObject(std::shared_ptr<Object> object);

private:
    void executeCustomComponents();
    void render();
    void lighting(Light::Type lightType);
    void printInfo(int windowWidth, int windowHeight);
    bool createSkybox();
    bool objectInsideFrustum(const Object* obj, const Camera* cam) const;

    std::shared_ptr<Application> app;

    GLFWwindow* window;
    ResourceManager manager;
    GUI gui;
    Input input;
    RenderSettings renderSettings;

    std::shared_ptr<Camera> camera;
    std::map<std::string, std::vector<Object*>> renderObjects;
    std::vector<std::vector<Object*>> lights;
    std::vector<std::shared_ptr<Object>> allObjects;

    std::shared_ptr<Object> skybox;

    Framebuffer fb1;
    Framebuffer fb2;
    Framebuffer* fb;
    Postprocess passthrough;

    double time;
};

} // moar

#endif // ENGINE

