#include "engine.h"
#include "renderer.h"
#include "material.h"
#include "globals.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <exception>
#include <utility>

namespace
{

void APIENTRY debugCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
                                    const GLchar* message, void* /*userParam*/)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    std::cerr << "OpenGL debug callback function" << std::endl;
    std::cerr << "    Message: "<< message << std::endl;

    std::cerr << "    Source: ";
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        std::cerr << "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cerr << "WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cerr << "SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cerr << "THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cerr << "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cerr << "OTHER";
        break;
    }
    std::cerr << std::endl;

    std::cerr << "    Type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cerr << "TYPE_ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cerr << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cerr << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cerr << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cerr << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cerr << "MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cerr << "PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cerr << "POP_GROUP";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cerr << "OTHER";
        break;
    default:
        std::cerr << "UNKNOWN";
        break;
    }
    std::cerr << std::endl;

    std::cerr << "    ID: " << id << std::endl;
    std::cerr << "    Severity: ";
    switch (severity){
    case GL_DEBUG_SEVERITY_LOW:
        std::cerr << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cerr << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cerr << "HIGH";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cerr << "NOTIFICATION";
        break;
    default:
        std::cerr << "UNKNOWN";
        break;
    }
    std::cerr << std::endl << std::endl;
}

void glfwErrorCallback(int error, const char* description )
{
    std::cerr << "ERROR GLFW: " << error << " " << description << std::endl;
}

} // anonymous

namespace moar
{

Engine::Engine() :
    window(nullptr),
    fb(nullptr),
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

    glfwSetErrorCallback(glfwErrorCallback);

    boost::property_tree::ptree pt;
    try {
        boost::property_tree::ini_parser::read_ini(settingsFile, pt);
        if (pt.get<int>("OpenGL.debug")) {
            DEBUG = true;
        }
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
        if (DEBUG) {
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        }
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
    renderSettings.windowWidth = windowWidth;
    renderSettings.windowHeight = windowHeight;

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
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(err));
        return false;
    }

    if (DEBUG && glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);        
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);
        glDebugMessageCallback(debugCallbackFunction, nullptr);
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

    Framebuffer::setWidth(renderSettings.windowWidth);
    Framebuffer::setHeight(renderSettings.windowHeight);
    bool framebuffersInitialized = fb1.init() && fb2.init();
    if (!framebuffersInitialized) {
        std::cerr << "ERROR: Framebuffer status is incomplete" << std::endl;
        return false;
    }

    // Todo: These should be in the application / camera.
    Postprocess offset("offset", manager.getShader("postproc/offset"));
    offset.setUniform("screensize", std::bind(glUniform2f, SCREEN_SIZE_LOCATION, renderSettings.windowWidth, renderSettings.windowHeight));
    postprocs.push_back(std::move(offset));
    Postprocess invert("invert", manager.getShader("postproc/invert"));
    postprocs.push_back(std::move(invert));
    Postprocess passthrough("passthrough", manager.getShader("postproc/passthrough"));
    postprocs.push_back(std::move(passthrough));

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

        DRAW_COUNT = 0;
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
    fb1.bind();
    glViewport(0, 0, renderSettings.windowWidth, renderSettings.windowHeight);
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
            if (!objectInsideFrustum(renderObj, camera.get())) {
                continue;
            }
            renderObj->prepareRender(true);
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
            if (!objectInsideFrustum(renderObj, camera.get())) {
                continue;
            }
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

    // Post-process ping-pong
    // Todo: Update post proc uniforms somewhere else. Probably in the application?
    postprocs.front().setUniform("time", std::bind(glUniform1f, TIME_LOCATION, time));
    GLuint renderedTex = fb1.getRenderedTexture();
    for (unsigned int i = 0; i < postprocs.size() - 1; ++i) {
        fb = i % 2 == 0 ? &fb2 : &fb1;
        fb->setPreviousFrame(renderedTex);
        fb->bind();        
        postprocs[i].bind();
        fb->activate();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        renderedTex = fb->getRenderedTexture();
    }
    postprocs.back().bind();
    fb = postprocs.size() % 2 == 0 ? &fb1 : &fb2;
    fb->setPreviousFrame(renderedTex);
    fb->activate();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
    material->setShader(renderSettings.skyboxShader);
    material->setTexture(texture, Material::TextureType::DIFFUSE, GL_TEXTURE_CUBE_MAP);

    Renderer* renderer = new Renderer();
    Model* model = getResourceManager()->getModel("cube.3ds");
    renderer->setModel(model);

    skybox.reset(new Object());
    skybox->addComponent(material);
    skybox->addComponent(renderer);
    return true;
}

bool Engine::objectInsideFrustum(const Object* obj, const Camera* cam) const
{
    Model* model = obj->getComponent<Renderer>()->getModel();
    glm::vec3 point = model->getCenterPoint();
    point = glm::vec3((*Object::view) * obj->getModelMatrix() * glm::vec4(point.x, point.y, point.z, 1.0f));
    float radius = model->getBoundingRadius();
    return cam->sphereInsideFrustum(point, radius);
}

} // moar
