#include "myapp.h"
#include "../engine/common/globals.h"
#include "../engine/model.h"
#include "../engine/material.h"

#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <memory>

MyApp::MyApp() :
    drawCount(&moar::G_DRAW_COUNT)
{
}

MyApp::~MyApp()
{
}

void MyApp::start()
{
    camera = engine->getCamera();
    resetCamera();
    input = engine->getInput();
    renderSettings = engine->getRenderSettings();
    time = engine->getTime();

#define SPONZA
//#define MONKEY
#define POINT_LIGHTS
//#define DIR_LIGHT
//#define POSTPROC

#ifdef SPONZA
    moar::Object* sponza = createRenderObject("sponza.obj");
    sponza->setScale(glm::vec3(0.004f, 0.004f, 0.004f));
#endif

#ifdef MONKEY
    monkey = engine->createObject();
    monkey->setShadowReceiver(false);
    monkey->addComponent<moar::Model>(engine->getResourceManager()->getModel("monkey.3ds"));
    moar::Material* m = engine->getResourceManager()->createMaterial();
    m->setTexture(engine->getResourceManager()->getTexture("spnza_bricks_a_diff.tga"), moar::Material::DIFFUSE, GL_TEXTURE_2D);
    m->setTexture(engine->getResourceManager()->getTexture("brickwork_nmap.png"), moar::Material::NORMAL, GL_TEXTURE_2D);
    m->setTexture(engine->getResourceManager()->getTexture("brickwork_bmap.png"), moar::Material::BUMP, GL_TEXTURE_2D);
    int shaderType = moar::Shader::DIFFUSE | moar::Shader::NORMAL | moar::Shader::BUMP;
    m->setShaderType(shaderType);
    for (auto& mo : monkey->getMeshObjects()) {
        mo.material = m;
    }
    monkey->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    monkey->setPosition(glm::vec3(-2.0f, 0.5f, 0.0f));
#endif

#ifdef POINT_LIGHTS
    light1a = createLight(glm::vec4(1.0f, 0.8f, 0.6f, 1.2f));
    light1b = createLight(glm::vec4(0.6f, 0.8f, 1.0f, 1.3f));
    light1b->getComponent<moar::Light>()->setShadowingEnabled(false);
    light2a = createLight(glm::vec4(0.6f, 1.0f, 0.8f, 1.2f));
    light2b = createLight(glm::vec4(1.0f, 0.6f, 0.8f, 1.3f));
    light2b->getComponent<moar::Light>()->setShadowingEnabled(false);
    // Lion light.
    light3 = createLight(glm::vec4(0.8f, 0.9f, 1.0f, 0.5f));
    light3->setPosition(glm::vec3(-5.0f, 0.5f, -0.4f));
#endif

#ifdef DIR_LIGHT
    moar::Object* dirLight = createLight(glm::vec4(0.8f, 0.9f, 1.0f, 0.5f), moar::Light::DIRECTIONAL);
    dirLight->setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    dirLight->setRotation(glm::vec3(-1.5f, 0.0f, 0.0f));
#endif

#ifdef POSTPROC
    offset = camera->addPostprocess("offset", engine->getResourceManager()->getShader("offset")->getProgram(), 1);
    offset->setUniform("screensize", std::bind(glUniform2f, moar::SCREEN_SIZE_LOCATION, renderSettings->windowWidth, renderSettings->windowHeight));
    camera->addPostprocess("invert", engine->getResourceManager()->getShader("invert")->getProgram(), 1);
#endif

    initGUI();
}

void MyApp::handleInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)  {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        quit();
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->move(camera->getForward() * input->getMovementSpeed() * time->getDelta());
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->move(-camera->getForward() * input->getMovementSpeed() * time->getDelta());
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->move(camera->getLeft() * input->getMovementSpeed() * time->getDelta());
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->move(-camera->getLeft() * input->getMovementSpeed() * time->getDelta());
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        resetCamera();
    }

    camera->rotate(moar::Object::UP, -input->getCursorDeltaX() * boost::math::constants::degree<double>());
    camera->rotate(moar::Object::LEFT, input->getCursorDeltaY() * boost::math::constants::degree<double>());
}

void MyApp::update()
{
    timeCounter += time->getDelta();
    if (timeCounter < 1.0) {
        ++fpsCounter;
    } else {
        fps = fpsCounter;
        timeCounter = 0.0;
        fpsCounter = 0;
    }
#ifdef POINT_LIGHTS
    float t = time->getTime();
    float sint = static_cast<float>(sin(t));
    light1a->setPosition(glm::vec3(1.5 + sin(t * 0.1), 1.8 + (sint * 0.3), 0.0f));
    light1b->setPosition(glm::vec3(1.5 - sin(t * 0.2), 1.7 + (sint * 0.5), 0.0f));
    light2a->setPosition(glm::vec3(-1.5 - sin(t * 0.1), 1.8 + (sint * 0.4), 0.0f));
    light2b->setPosition(glm::vec3(-1.5 + sin(t * 0.3), 1.7 + (sint * 0.3), 0.0f));
    light3->move(glm::vec3(0.0f, sin(1.5f * t) * 0.01f, sint * 0.03f));
#endif
    position = camera->getPosition();

#ifdef POSTPROC
    offset->setUniform("time", std::bind(glUniform1f, moar::TIME_LOCATION, glfwGetTime()));
#endif
}

void MyApp::initGUI()
{
    bar = TwNewBar("GUI");
    TwAddVarRO(bar, "fps", TW_TYPE_INT32, &fps, "");
    TwAddVarRO(bar, "draw count", TW_TYPE_UINT32, drawCount, "");
    TwAddVarRO(bar, "position", TW_TYPE_DIR3F, &position, "");
}

moar::Object* MyApp::createRenderObject(const std::string& modelName)
{
    moar::Object* renderObj= engine->createObject();
    moar::Model* model = engine->getResourceManager()->getModel(modelName);
    renderObj->addComponent<moar::Model>(model);
    return renderObj;
}

moar::Object* MyApp::createLight(const glm::vec4& color, moar::Light::Type type)
{
    moar::Object* light = engine->createObject();
    moar::Light* lightComponent = light->addComponent<moar::Light>();
    lightComponent->setColor(color);
    lightComponent->setType(type);
    light->addComponent<moar::Model>(engine->getResourceManager()->getModel("sphere.3ds"));
    light->setScale(glm::vec3(0.02f, 0.02f, 0.02f));
    light->setShadowCaster(false);
    light->setShadowReceiver(false);
    return light;
}

void MyApp::resetCamera()
{
    camera->setPosition(glm::vec3(0.0f, 0.5f, 0.5f));
    camera->setRotation(glm::vec3(0.0f, 0.9f, 0.0f));
}
