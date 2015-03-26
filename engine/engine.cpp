#include "engine.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace moar {

Engine::Engine(const std::string& settingsFile) :
    settingsFile(settingsFile),
    timeLimit(false, 0.0) {
}

bool Engine::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(settingsFile, pt);
    int openglMajor = pt.get<int>("OpenGL.major");
    int openglMinor = pt.get<int>("OpenGL.minor");
    int screenWidth = pt.get<int>("Window.width");
    int screenHeight = pt.get<int>("Window.height");
    timeLimit.first = pt.get<bool>("Engine.useTimeLimit");
    timeLimit.second = pt.get<double>("Engine.timeLimit");

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    window = glfwCreateWindow(screenWidth, screenHeight, "moar-gl", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, 300, 100);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

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
    return true;
}

Engine::~Engine() {
    delete app;
    glfwTerminate();
}

void Engine::execute() {
    app->initialize();
    while (app->isRunning()) {
        app->run();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
            app->quit();
        }
        if (timeLimit.first && glfwGetTime() >= timeLimit.second) {
            app->quit();
        }
    }
}

} // moar
