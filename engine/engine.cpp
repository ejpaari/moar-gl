#include "engine.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace moar
{

Engine::Engine() :
    useTimeLimit(false),
    timeLimit(0.0)
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
    int openglMajor = pt.get<int>("OpenGL.major");
    int openglMinor = pt.get<int>("OpenGL.minor");

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    int screenWidth = pt.get<int>("Window.width");
    int screenHeight = pt.get<int>("Window.height");
    window = glfwCreateWindow(screenWidth, screenHeight, pt.get<std::string>("Window.title").c_str(), NULL, NULL);
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
    double sensitivity = pt.get<double>("Input.sensitivity");
    float movementSpeed = pt.get<double>("Input.movementSpeed");
    app->getInput()->setSensitivity(sensitivity);
    app->getInput()->setMovementSpeed(movementSpeed);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    useTimeLimit = pt.get<bool>("Engine.useTimeLimit");
    timeLimit = pt.get<double>("Engine.timeLimit");
    manager.setShaderPath(pt.get<std::string>("Engine.shaderPath"));
    manager.setModelPath(pt.get<std::string>("Engine.modelPath"));
    manager.setTexturePath(pt.get<std::string>("Engine.texturePath"));

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    printInfo(screenWidth, screenHeight);

    return true;
}

void Engine::execute()
{
    app->start();
    double x = 0.0;
    double y = 0.0;
    while (app->isRunning()) {
        glfwGetCursorPos(window, &x, &y);
        app->getInput()->setCursorPosition(x, y);

        app->handleInput(window);
        app->update(glfwGetTime());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        app->render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            app->quit();
        }
        if (useTimeLimit && glfwGetTime() >= timeLimit) {
            app->quit();
        }
    }
}

void Engine::printInfo(int screenWidth, int screenHeight)
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

    std::cout << "Window resolution: " << screenWidth << " x " << screenHeight << std::endl;
}

} // moar
