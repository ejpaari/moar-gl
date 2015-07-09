#include "myapp.h"
#include "../engine/globals.h"
#include "../engine/renderer.h"
#include "../engine/material.h"
#include "../engine/light.h"

#include <boost/math/constants/constants.hpp>
#include <cmath>

MyApp::MyApp() :
    camera(nullptr),
    input(nullptr),
    bar(nullptr),
    rotationAxis(0.0f, 1.0f, 0.0f),
    rotationSpeed(0.5f),
    fps(0),
    fpsCounter(0),
    drawCount(&moar::DRAW_COUNT),
    timeCounter(0.0)
{    
    monkey1 = nullptr;
    monkey2 = nullptr;
    light1 = nullptr;
    light2 = nullptr;
    light3 = nullptr;
    icosphere = nullptr;
}

MyApp::~MyApp()
{
}

void MyApp::start()
{
    camera = engine->getCamera();
    input = engine->getInput();

    monkey1 = createRenderObject("diffuse", "monkey.3ds", "checker.png");
    monkey1->setPosition(glm::vec3(0.0f, 0.0f, -3.0f));

    monkey2 = createRenderObject("specular", "monkey.3ds", "checker.png");
    monkey2->setPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    moar::Material* mat = monkey2->getComponent<moar::Material>();
    mat->setSpecularity(50.0f);

    icosphere = createRenderObject("normalmap", "icosphere.3ds", "brick.png");
    icosphere->setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
    mat = icosphere->getComponent<moar::Material>();
    mat->setSpecularity(100.0f);
    mat->setTexture(engine->getResourceManager()->getTexture("brick_nmap.png"), moar::Material::TextureType::NORMAL, GL_TEXTURE_2D);

    light1 = createLight(glm::vec4(0.0f, 1.0f, 0.0f, 5.0f));
    light1->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
    light2 = createLight(glm::vec4(1.0f, 0.0f, 0.0f, 15.0f));
    light2->setPosition(glm::vec3(0.0f, -3.0f, 0.0f));
    light3 = createLight(glm::vec4(0.0f, 0.0f, 1.0f, 10.0f));
    light3->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    initGUI();
}

void MyApp::handleInput(GLFWwindow *window)
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
        camera->move(camera->getForward() * input->getMovementSpeed());
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->move(-camera->getForward() * input->getMovementSpeed());
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->move(camera->getLeft() * input->getMovementSpeed());
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->move(-camera->getLeft() * input->getMovementSpeed());
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        camera->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    camera->rotate(moar::Object::UP, -input->getCursorDeltaX() * boost::math::constants::degree<double>());
    camera->rotate(moar::Object::LEFT, input->getCursorDeltaY() * boost::math::constants::degree<double>());
}

void MyApp::update(double time, double deltaTime)
{
    timeCounter += deltaTime;
    if (timeCounter < 1.0) {
        ++fpsCounter;
    } else {
        fps = fpsCounter;
        timeCounter = 0.0;
        fpsCounter = 0;
    }

    monkey1->rotate(rotationAxis, rotationSpeed * boost::math::constants::degree<double>());

    light1->move(glm::vec3(0.0f, sin(time) * 0.1f, 0.0f));
    light2->move(glm::vec3(0.0f, cos(time) * 0.1f, 0.0f));
}

void MyApp::initGUI()
{
    bar = TwNewBar("TweakBar");
    TwAddVarRW(bar, "axis", TW_TYPE_DIR3F, &rotationAxis, "");
    TwAddVarRW(bar, "speed", TW_TYPE_FLOAT, &rotationSpeed, "");
    TwAddVarRO(bar, "fps", TW_TYPE_INT32, &fps, "");
    TwAddVarRO(bar, "draw count", TW_TYPE_UINT32, drawCount, "");
}

moar::Object* MyApp::createRenderObject(const std::string& shaderName, const std::string& modelName, const std::string& textureName)
{
    GLuint shader = engine->getResourceManager()->getShader(shaderName);
    GLuint texture = 0;
    if (!textureName.empty()) {
        texture = engine->getResourceManager()->getTexture(textureName);
    }
    moar::Material* material = new moar::Material();
    material->setShader(shader);
    material->setTexture(texture, moar::Material::TextureType::DIFFUSE, GL_TEXTURE_2D);

    moar::Renderer* renderer = new moar::Renderer();
    moar::Model* model = engine->getResourceManager()->getModel(modelName);
    renderer->setModel(model);

    moar::Object* renderObj = new moar::Object();
    renderObj->addComponent(material);
    renderObj->addComponent(renderer);
    engine->addObject(renderObj);
    return renderObj;
}

moar::Object* MyApp::createLight(const glm::vec4& color)
{
    moar::Light* lightComponent = new moar::Light();
    lightComponent->setColor(color);
    moar::Object* light = new moar::Object();
    light->addComponent(lightComponent);
    engine->addObject(light);
    return light;
}
