#include "engine.h"
#include "renderer.h"
#include "material.h"
#include "constants.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <exception>

namespace moar
{

Engine::Engine() :
    window(nullptr),
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
        std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
        return false;
    }

    boost::property_tree::ptree pt;
    try {
        boost::property_tree::ini_parser::read_ini(settingsFile, pt);
    } catch (std::exception& e) {
        std::cerr << "ERROR: Could not load setting file; " << settingsFile << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }

    try {
        glfwWindowHint(GLFW_SAMPLES, pt.get<int>("OpenGL.multisampling"));
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, pt.get<int>("OpenGL.major"));
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, pt.get<int>("OpenGL.minor"));
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "ERRROR: Could not load OpenGL version info from the .ini-file" << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }

    int windowWidth = 800;
    int windowHeight = 600;
    try {
        windowWidth = pt.get<int>("Window.width");
        windowHeight = pt.get<int>("Window.height");
        window = glfwCreateWindow(windowWidth, windowHeight, pt.get<std::string>("Window.title").c_str(), NULL, NULL);
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "WARNING: Could not load window info from the .ini-file" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    if (window == NULL) {
        std::cerr << "ERROR: Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    int windowPosX = 0;
    int windowPosY = 0;
    try {
        windowPosX = pt.get<int>("Window.Xposition");
        windowPosY = pt.get<int>("Window.Yposition");
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "WARNING: Could not load window position from the .ini-file" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, windowPosX, windowPosY);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR: Failed to initialize GLEW" << std::endl;
        return false;
    }

    printInfo(windowWidth, windowHeight);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    try {
        double sensitivity = pt.get<double>("Input.sensitivity");
        float movementSpeed = pt.get<double>("Input.movementSpeed");
        input.setSensitivity(sensitivity);
        input.setMovementSpeed(movementSpeed);
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "WARNING: Could not load input info from the .ini-file" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    if (!gui.init(window)) {
        std::cerr << "ERROR: Failed to initialize AntTweakBar" << std::endl;
        return false;
    }

    try {
        manager.setShaderPath(pt.get<std::string>("Engine.shaderPath"));
        manager.setModelPath(pt.get<std::string>("Engine.modelPath"));
        manager.setTexturePath(pt.get<std::string>("Engine.texturePath"));
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "ERROR: Could not load resource path info from the .ini-file" << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }

    // Todo: Multiple cameras.
    camera.reset(new Camera());
    Object::view = camera->getViewMatrixPointer();
    Object::projection = camera->getProjectionMatrixPointer();

    if (!renderSettings.loadSettings(pt, manager)) {
        std::cerr << "WARNING: Failed to load render settings" << std::endl;
    }

    if (!createSkybox()) {
        std::cerr << "WARNING: Failed to create the skybox" << std::endl;
    }

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
    }

    gui.uninit();
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
    glEnable(GL_DEPTH_TEST);

    // Ambient.
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glUseProgram(renderSettings.ambientShader);
    glUniform3f(AMBIENT_LOCATION, renderSettings.ambientColor.x, renderSettings.ambientColor.y, renderSettings.ambientColor.z);
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
        glUseProgram(renderSettings.skyboxShader);
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

    std::cout << "Window resolution: " << windowWidth << " x " << windowHeight << std::endl << std::endl;
}

bool Engine::createSkybox()
{
    if (!renderSettings.isLoaded()) {
        return false;
    }
    GLuint texture = manager.getTexture(renderSettings.skyboxTextures);
    Material* material = new Material();
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
