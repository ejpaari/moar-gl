#include "myapp.h"
#include "../engine/common/globals.h"
#include "../engine/model.h"
#include "../engine/material.h"

#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <memory>

MyApp::MyApp() :
    camera(nullptr),
    input(nullptr),
    renderSettings(nullptr),
    time(nullptr),
    offset(nullptr),
    bar(nullptr),
    monkey(nullptr),
    icosphere(nullptr),
    light1(nullptr),
    light2(nullptr),
    dirLight(nullptr),
    fps(0),
    fpsCounter(0),
    drawCount(&moar::G_DRAW_COUNT),
    timeCounter(0.0)
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

    moar::Object* sponza = createRenderObject("sponza.obj");
    sponza->setScale(glm::vec3(0.004f, 0.004f, 0.004f));

    //    monkey = createRenderObject("specular", "monkey.3ds", "checker.png");
    //    monkey->setPosition(glm::vec3(3.0f, 0.1f, 0.0f));
    //    monkey->setName("specular_monkey");
    //    moar::Material* mat = monkey->getComponent<moar::Material>();
    //    mat->setUniform("specularity", std::bind(glUniform1f, moar::SPECULAR_LOCATION, 50.0f), moar::SPECULAR_LOCATION);

    //    icosphere = createRenderObject("normalmap", "icosphere.3ds", "brick.png");
    //    icosphere->setPosition(glm::vec3(-3.0f, 2.0f, 0.0f));
    //    icosphere->setName("icosphere");
    //    mat = icosphere->getComponent<moar::Material>();
    //    mat->setTexture(engine->getResourceManager()->getTexture("brick_nmap.png"), moar::Material::TextureType::NORMAL, GL_TEXTURE_2D);

    light1 = createLight(glm::vec4(0.8f, 1.0f, 0.8f, 1.5f));
    light1->setPosition(glm::vec3(1.0f, 2.5f, 0.0f));
    light2 = createLight(glm::vec4(1.0f, 1.0f, 0.8f, 1.5f));
    light2->setPosition(glm::vec3(-1.0f, 2.5f, 0.0f));
    //    light2->getComponent<moar::Light>()->setShadowingEnabled(false);
    //    dirLight = createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.1f), moar::Light::DIRECTIONAL);
    //    dirLight->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    //    dirLight->setRotation(glm::vec3(-1.0f, 0.0f, 0.0f));
    //    offset = camera->addPostprocess("offset", engine->getResourceManager()->getShader("offset")->getProgram(), 1);
    //    offset->setUniform("screensize", std::bind(glUniform2f, moar::SCREEN_SIZE_LOCATION, renderSettings->windowWidth, renderSettings->windowHeight));
    //    camera->addPostprocess("invert", engine->getResourceManager()->getShader("invert")->getProgram(), 1);

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

    //    monkey1->rotate(rotationAxis, rotationSpeed * boost::math::constants::degree<double>());

    double t = time->getTime();
    light1->move(glm::vec3(0.0f, sin(t) * 0.01f, 0.0f));
    light2->move(glm::vec3(0.0f, cos(t) * 0.02f, 0.0f));

    //    offset->setUniform("time", std::bind(glUniform1f, moar::TIME_LOCATION, glfwGetTime()));
}

void MyApp::initGUI()
{
    bar = TwNewBar("GUI");
    TwAddVarRO(bar, "fps", TW_TYPE_INT32, &fps, "");
    TwAddVarRO(bar, "draw count", TW_TYPE_UINT32, drawCount, "");
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
    return light;
}

void MyApp::resetCamera()
{
    camera->setPosition(glm::vec3(0.0f, 0.5f, 0.5f));
    camera->setRotation(glm::vec3(0.0f, 0.9f, 0.0f));
}
