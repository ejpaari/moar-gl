#include "engine.h"
#include "renderer.h"
#include "material.h"
#include "constants.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace moar
{

Engine::Engine() :
    window(nullptr),
    skybox(nullptr),
    skyboxShader(0),
    ambientShader(0),
    ambientColor(0.01f, 0.01f, 0.01f),
    useTimeLimit(false),
    timeLimit(0.0),
    time(0.0)
{    
}

Engine::~Engine()
{
    glfwTerminate();
}

void Engine::setApplication(std::shared_ptr<Application> application)
{
     app = application;
     app->setEngine(this);
}

bool Engine::init(const std::string& settingsFile)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(settingsFile, pt);

    glfwWindowHint(GLFW_SAMPLES, pt.get<int>("OpenGL.multisampling"));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, pt.get<int>("OpenGL.major"));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, pt.get<int>("OpenGL.minor"));
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    int windowWidth = pt.get<int>("Window.width");
    int windowHeight = pt.get<int>("Window.height");
    window = glfwCreateWindow(windowWidth, windowHeight, pt.get<std::string>("Window.title").c_str(), NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    int windowPosX = pt.get<int>("Window.Xposition");
    int windowPosY = pt.get<int>("Window.Yposition");
    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, windowPosX, windowPosY);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double sensitivity = pt.get<double>("Input.sensitivity");
    float movementSpeed = pt.get<double>("Input.movementSpeed");
    input.setSensitivity(sensitivity);
    input.setMovementSpeed(movementSpeed);

    if (!gui.init(window)) {
        std::cerr << "Failed to initialize AntTweakBar" << std::endl;
        return false;
    }

    useTimeLimit = pt.get<bool>("Engine.useTimeLimit");
    timeLimit = pt.get<double>("Engine.timeLimit");
    manager.setShaderPath(pt.get<std::string>("Engine.shaderPath"));
    manager.setModelPath(pt.get<std::string>("Engine.modelPath"));
    manager.setTexturePath(pt.get<std::string>("Engine.texturePath"));

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    printInfo(windowWidth, windowHeight);

    // Todo: Multiple cameras.
    camera.reset(new Camera());
    Object::view = camera->getViewMatrixPointer();
    Object::projection = camera->getProjectionMatrixPointer();

    std::vector<std::string> skyboxTextures = {"sb_px.png", "sb_nx.png", "sb_py.png", "sb_ny.png", "sb_pz.png", "sb_nz.png"};
    if (!createSkybox(skyboxTextures)) {
        std::cerr << "Failed to create the skybox" << std::endl;
    }

    ambientShader = manager.getShader("ambient");

    return true;
}

void Engine::execute()
{
    app->start();
    double x = 0.0;
    double y = 0.0;
    while (app->isRunning()) {
        glfwGetCursorPos(window, &x, &y);
        input.setCursorPosition(x, y);

        app->handleInput(window);
        app->update(glfwGetTime(), glfwGetTime() - time);        
        time = glfwGetTime();
        executeCustomComponents();        
        render();
        gui.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            app->quit();
        }
        if (useTimeLimit && glfwGetTime() >= timeLimit) {
            app->quit();
            gui.uninit();
        }
    }
}

ResourceManager* Engine::getResourceManager()
{
    return &manager;
}

Camera* Engine::getCamera()
{
    return camera.get();
}

Input* Engine::getInput()
{
    return &input;
}

void Engine::addObject(Object* object)
{
    std::shared_ptr<Object> obj(object);
    allObjects.push_back(obj);

    if (object->hasComponent("Renderer")) {
        GLuint shaderProgram = object->getComponent<Material>()->getShader();
        renderObjects[shaderProgram].push_back(object);
    }
    if (object->hasComponent("Light")) {
        lights.push_back(object);
    }
}

void Engine::executeCustomComponents()
{
    for (unsigned int i = 0; i < allObjects.size(); ++i) {
        allObjects[i]->executeCustomComponents();
    }
}

void Engine::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Ambient.
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glUseProgram(ambientShader);
    glUniform3f(AMBIENT_LOCATION, ambientColor.x, ambientColor.y, ambientColor.z);
    for (auto renderObjs : renderObjects) {
        for (auto renderObj : renderObjs.second) {
            renderObj->prepareRender();
            renderObj->render();
        }
    }

    // Lighting.
    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_ONE, GL_ONE);
    for (auto renderObjs : renderObjects) {
        glUseProgram(renderObjs.first);
        for (auto renderObj : renderObjs.second) {
            renderObj->prepareRender();
            for (unsigned int i = 0; i < lights.size(); ++i) {
                lights[i]->prepareLight();
                renderObj->render();
            }
        }
    }

    if (skybox) {
        glDisable(GL_BLEND);
        glCullFace(GL_FRONT);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShader);
        skybox->setPosition(camera->getPosition());
        skybox->prepareRender();
        skybox->render();
    }
}

void Engine::printInfo(int windowWidth, int windowHeight)
{
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "GFX: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: "  << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    std::cout << "Resolution: " << mode->width << " x " << mode->height << std::endl;
    std::cout << "Refresh rate: " << mode->refreshRate << " Hz" << std::endl;
    int monitorWidth, monitorHeight;
    glfwGetMonitorPhysicalSize(primaryMonitor, &monitorWidth, &monitorHeight);
    std::cout << "Monitor size: " << monitorWidth << "mm x " << monitorHeight << "mm" << std::endl << std::endl;

    std::cout << "Window resolution: " << windowWidth << " x " << windowHeight << std::endl;
}

bool Engine::createSkybox(const std::vector<std::string>& files)
{
    skyboxShader = manager.getShader("skybox");
    GLuint texture = 0;
    if (!files.empty()) {
        texture = manager.getTexture(files);
    }
    Material* material = new Material();
    material->setShader(skyboxShader);
    material->setTexture(texture, Material::TextureType::DIFFUSE);

    Renderer* renderer = new Renderer();
    Model* model = getResourceManager()->getModel("cube.3ds");
    renderer->setModel(model);

    skybox.reset(new Object());
    skybox->addComponent(material);
    skybox->addComponent(renderer);
    return true;
}

} // moar
