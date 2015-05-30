#ifndef ENGINE
#define ENGINE

#include "application.h"
#include "resourcemanager.h"
#include "gui.h"
#include "input.h"
#include "camera.h"
#include "object.h"

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

    void setApplication(std::shared_ptr<Application> application);
    bool init(const std::string& settingsFile);
    void execute();

    ResourceManager* getResourceManager();
    Camera* getCamera();
    Input* getInput();
    void addObject(Object* object);

private:
    void executeCustomComponents();
    void render();
    void printInfo(int windowWidth, int windowHeight);
    bool createSkybox(const std::vector<std::string>& files);

    std::shared_ptr<Application> app;

    GLFWwindow* window;
    ResourceManager manager;
    GUI gui;
    Input input;

    std::shared_ptr<Camera> camera;
    std::map<GLuint, std::vector<Object*>> renderObjects;
    std::vector<Object*> lights;
    std::vector<std::shared_ptr<Object>> allObjects;

    std::shared_ptr<Object> skybox;
    GLuint skyboxShader;

    GLuint ambientShader;
    glm::vec3 ambientColor;

    bool useTimeLimit;
    double timeLimit;
    double time;
};

} // moar

#endif // ENGINE

