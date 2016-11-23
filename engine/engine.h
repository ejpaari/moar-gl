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
#include "postframebuffer.h"
#include "depthmapdir.h"
#include "depthmappoint.h"
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

    Object* createObject(const std::string& name = "");
    bool loadLevel(const std::string& level);

    unsigned int getDrawCount() const;

private:
    void resetLevel();
    void render();
    void lighting(Light::Type lightType);
    void updateObjectContainers();
    void updateObjects();
    void printInfo(int windowWidth, int windowHeight);
    bool createSkybox();
    bool objectInsideFrustum(const Object::MeshObject& mo) const;
    void setPostFramebuffer();

    std::shared_ptr<Application> app;

    GLFWwindow* window = nullptr;
    ResourceManager manager;
    GUI gui;
    Input input;
    RenderSettings renderSettings;
    Time time;

    using MaterialId = int;
    using ShaderType = int;
    using MeshMap = std::map<MaterialId, std::vector<Object::MeshObject>>;
    std::map<ShaderType, MeshMap> renderMeshes;
    std::vector<std::vector<Object*>> lights;
    std::vector<std::shared_ptr<Object>> allObjects;
    std::shared_ptr<Camera> camera;

    std::shared_ptr<Object> skybox;
    std::unordered_set<unsigned int> objectsInFrustum;

    DepthMapDirectional depthMapDir;
    DepthMapPoint depthMapPoint;
    Framebuffer fb;
    PostFramebuffer postBuffer1;
    PostFramebuffer postBuffer2;
    PostFramebuffer blitBuffer1;
    PostFramebuffer blitBuffer2;
    PostFramebuffer* postBuffer = nullptr;
    const Shader* shader = nullptr;
    Postprocess passthrough;
};

} // moar

#endif // ENGINE_H

