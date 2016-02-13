#include "engine.h"
#include "model.h"
#include "material.h"
#include "common/globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <exception>
#include <utility>
#include <algorithm>

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

Engine::Engine()
{
}

Engine::~Engine()
{
    glfwTerminate();
}

void Engine::setApplication(Application* application)
{
    app.reset(application);
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
#ifdef DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
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
#ifdef DEBUG
    if (glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);        
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);
        glDebugMessageCallback(debugCallbackFunction, nullptr);
    }
#endif

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

    std::string shaderInfoFile = "";
    try {
        manager.setShaderPath(pt.get<std::string>("Engine.shaderPath"));
        manager.setModelPath(pt.get<std::string>("Engine.modelPath"));
        manager.setTexturePath(pt.get<std::string>("Engine.texturePath"));
        shaderInfoFile = pt.get<std::string>("Engine.shaders");
    } catch (boost::property_tree::ptree_error& e) {
        std::cerr << "ERROR: Could not load resource path info from the .ini-file" << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }

    if (!manager.loadShaders(shaderInfoFile)) {
        return false;
    }

    camera.reset(new Camera());
    Object::view = camera->getViewMatrixPointer();
    Object::projection = camera->getProjectionMatrixPointer();

    if (!renderSettings.loadSettings(pt, manager)) {
        std::cerr << "WARNING: Failed to load render settings" << std::endl;
    }

    if (!createSkybox()) {
        std::cerr << "WARNING: Failed to create the skybox" << std::endl;
    }

    depthMapDir.setWidth(renderSettings.windowWidth);
    depthMapDir.setHeight(renderSettings.windowHeight);
    if (!depthMapDir.init()) {
        return false;
    }

    int size = std::max(renderSettings.windowWidth, renderSettings.windowHeight);
    depthMapPoint.setWidth(size);
    depthMapPoint.setHeight(size);
    if (!depthMapPoint.init()) {
        return false;
    }

    Framebuffer::setWidth(renderSettings.windowWidth);
    Framebuffer::setHeight(renderSettings.windowHeight);
    bool framebuffersInitialized = fb1.init() && fb2.init();
    if (!framebuffersInitialized) {
        std::cerr << "ERROR: Framebuffer status is incomplete" << std::endl;
        return false;
    }

    passthrough = Postprocess("passthrough", manager.getShader("passthrough")->getProgram(), 0);

    lights.resize(Light::Type::NUM_TYPES);

    Material* material = manager.createMaterial();
    Object::setMeshDefaultMaterial(material);
    GLuint texture = manager.getTexture("brick.png");
    material->setTexture(texture, moar::Material::TextureType::DIFFUSE, GL_TEXTURE_2D);
    material->setShaderType("diffuse");

#ifdef DEBUG
    std::cout << "\nTHIS PROGRAM IS EXECUTED WITH THE DEBUG FLAG\n\n";
#endif

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

        time.update();

        app->handleInput(window);
        app->update();

        updateObjectContainers();
        updateModelMatrices();
        render();
        gui.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            app->quit();
        }

        G_DRAW_COUNT = 0;
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

RenderSettings* Engine::getRenderSettings()
{
    return &renderSettings;
}

Time*Engine::getTime()
{
    return &time;
}

Object* Engine::createObject()
{
    std::shared_ptr<moar::Object> obj(new moar::Object());
    allObjects.push_back(obj);
    return obj.get();
}

void Engine::render()
{
    fb = &fb1;
    fb->bind();
    objectsInFrustum.clear();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    Object::setViewMatrixUniform();

    // Ambient.
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    shader = renderSettings.ambientShader;
    glUseProgram(renderSettings.ambientShader->getProgram());
    glUniform3f(AMBIENT_LOCATION, renderSettings.ambientColor.x, renderSettings.ambientColor.y, renderSettings.ambientColor.z);
    for (auto& shaderMeshMap : renderMeshes) {
        for (auto& meshMap : shaderMeshMap .second) {
            manager.getMaterial(meshMap.first)->setMaterialUniforms(shader);
            for (auto& meshObject : meshMap.second) {
                if (!objectInsideFrustum(meshObject)) {
                   continue;
                }
                meshObject.parent->setTransformationUniforms(shader);
                meshObject.mesh->render();
                objectsInFrustum.insert(meshObject.mesh->getId());
            }
        }
    }

    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_ONE, GL_ONE);

    // Lighting.
    lighting(Light::DIRECTIONAL);
    lighting(Light::POINT);

    glDisable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);

    // Skybox.
    if (skybox) {
        glCullFace(GL_FRONT);
        shader = renderSettings.skyboxShader;
        glUseProgram(shader->getProgram());
        skybox->setPosition(camera->getPosition());
        skybox->setTransformationUniforms(shader);
        for (auto& meshObject : skybox->getMeshObjects()) {
            meshObject.material->setMaterialUniforms(shader);
            meshObject.mesh->render();
        }
    }

    // Post-process ping-pong    
    GLuint renderedTex = fb->getRenderedTexture();
    const std::list<Postprocess>& postprocs = camera->getPostprocesses();
    int i = 0;
    for (auto iter = postprocs.begin(); iter != postprocs.end(); ++iter) {
        fb = i % 2 == 0 ? &fb2 : &fb1;
        fb->setPreviousFrame(renderedTex);
        fb->bind();
        iter->bind();
        fb->activate();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        renderedTex = fb->getRenderedTexture();
        ++i;
    }
    passthrough.bind();
    fb = postprocs.size() % 2 == 0 ? &fb1 : &fb2;
    fb->setPreviousFrame(renderedTex);
    fb->activate();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Engine::lighting(Light::Type lightType)
{
    DepthMap* depthMap = nullptr;        
    if (lightType == Light::DIRECTIONAL) {
        depthMap = &depthMapDir;        
    } else {
        depthMap = &depthMapPoint;
    }

    for (auto& light : lights[lightType]) {
        shader = manager.getShader("depthmap", lightType);
        glUseProgram(shader->getProgram());
        Light* lightComp = light->getComponent<Light>();
        bool shadowingEnabled = lightComp->isShadowingEnabled();
        lightComp->setLightUniforms(light->getPosition(), light->getForward());
        depthMap->bind(light->getPosition(), light->getForward());
        if (shadowingEnabled) {
            for (auto& shaderMeshMap : renderMeshes) {
                for (auto& meshMap : shaderMeshMap.second) {
                    for (auto& meshObject : meshMap.second) {
                        if (meshObject.parent->isShadowCaster()) {
                            meshObject.parent->setTransformationUniforms(shader);
                            meshObject.mesh->render();
                        }
                    }
                }
            }
        }

        fb->bind();

        for (auto& shaderMeshMap : renderMeshes) {
            shader = manager.getShader(shaderMeshMap.first, lightType);
            glUseProgram(shader->getProgram());
            if (shader->hasUniform(FAR_CLIP_DISTANCE_LOCATION)) {
                glUniform1f(FAR_CLIP_DISTANCE_LOCATION, camera->getFarClipDistance());
            }
            depthMap->activate();
            for (auto& meshMap : shaderMeshMap.second) {
                manager.getMaterial(meshMap.first)->setMaterialUniforms(shader);
                for (auto& meshObject : meshMap.second) {
                    if (objectsInFrustum.find(meshObject.mesh->getId()) == objectsInFrustum.end()) {
                        continue;
                    }
                    meshObject.parent->setTransformationUniforms(shader);
                    meshObject.mesh->render();
                }
            }
        }
    }
}

void Engine::updateObjectContainers()
{
    if (!G_COMPONENT_CHANGED) {
        return;
    }

    for (const auto obj : allObjects) {
        for (auto& meshObject : obj->getMeshObjects()) {
            if (meshObject.material != nullptr) {
                ShaderType shaderType = meshObject.material->getShaderType();
                MaterialId materialId = meshObject.material->getId();
                std::vector<Object::MeshObject>& meshes = renderMeshes[shaderType][materialId];
                if (std::find(meshes.begin(), meshes.end(), meshObject) == meshes.end()) {
                    meshes.push_back(meshObject);
                }
            }
        }
        if (obj->hasComponent<Light>()) {
            Light::Type type = obj->getComponent<Light>()->getLightType();
            std::vector<Object*>& objs = lights[type];
            if (std::find(objs.begin(), objs.end(), obj.get()) == objs.end()) {
                lights[type].push_back(obj.get());
            }
        }
    }
    for (auto& objs : lights) {
        objs.erase(std::remove_if(objs.begin(), objs.end(), [](Object* obj){ return !obj->hasComponent<Light>(); }),
                objs.end());
    }

    for (auto& shaderMeshMap : renderMeshes) {
        for (auto& meshMap : shaderMeshMap.second) {
            std::vector<Object::MeshObject>& meshes = meshMap.second;
            meshes.erase(std::remove_if(meshes.begin(), meshes.end(), [](Object::MeshObject mo){ return mo.material == nullptr; }),
                    meshes.end());
        }
    }
    G_COMPONENT_CHANGED = false;
}

void Engine::updateModelMatrices()
{
    for (const std::shared_ptr<Object>& obj : allObjects) {
        obj->updateModelMatrix();
    }
    skybox->updateModelMatrix();
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

    skybox.reset(new Object());
    skybox->addComponent<Model>(getResourceManager()->getModel("cube.3ds"));

    Material* material = manager.createMaterial();
    GLuint texture = manager.getCubeTexture(renderSettings.skyboxTextures);
    material->setTexture(texture, Material::TextureType::DIFFUSE, GL_TEXTURE_CUBE_MAP);
    for (auto& meshObject : skybox->getMeshObjects()) {
        meshObject.material = material;
    }

    skybox->setShadowCaster(false);
    skybox->setShadowReceiver(false);

    return true;
}

bool Engine::objectInsideFrustum(const Object::MeshObject& mo) const
{
    glm::vec3 point = mo.mesh->getCenterPoint();
    point = glm::vec3((*Object::view) * mo.parent->getModelMatrix() * glm::vec4(point.x, point.y, point.z, 1.0f));
    glm::vec3 scale = mo.parent->getScale();
    float scaleMultiplier = std::max(std::max(scale.x, scale.y), scale.z);
    float radius = mo.mesh->getBoundingRadius() * scaleMultiplier ;
    return camera->sphereInsideFrustum(point, radius);
}

} // moar
