#ifndef ENGINE_H
#define ENGINE_H

#include "application.h"
#include "resourcemanager.h"
#include "gui.h"
#include "input.h"
#include "rendersettings.h"
#include "time.h"
#include "camera.h"
#include "object.h"
#include "light.h"
#include "framebuffer.h"
#include "depthmap_dir.h"
#include "depthmap_point.h"
#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
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
    Time* getTime();

    Object* createObject();

private:
    void render();
    void lighting(Light::Type lightType);
    void updateObjectContainers();
    void updateModelMatrices();
    void printInfo(int windowWidth, int windowHeight);
    bool createSkybox();
    bool objectInsideFrustum(const Object::MeshObject& mo) const;

    std::shared_ptr<Application> app;

    GLFWwindow* window = nullptr;
    ResourceManager manager;
    GUI gui;
    Input input;
    RenderSettings renderSettings;
    Time time;

    using MaterialId = int;
    using ShaderType = std::string;
    using MeshMap = std::map<MaterialId, std::vector<Object::MeshObject>>;
    std::map<ShaderType, MeshMap> renderMeshes;
    std::vector<std::vector<Object*>> lights;
    std::vector<std::shared_ptr<Object>> allObjects;
    std::shared_ptr<Camera> camera;

    std::shared_ptr<Object> skybox;
    std::unordered_set<unsigned int> objectsInFrustum;

    DepthMapDirectional depthMapDir;
    DepthMapPoint depthMapPoint;
    Framebuffer fb1;
    Framebuffer fb2;
    Framebuffer* fb = nullptr;
    const Shader* shader = nullptr;
    Postprocess passthrough;
};

} // moar

#endif // ENGINE_H

