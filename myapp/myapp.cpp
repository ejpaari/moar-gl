#include "myapp.h"
#include "../engine/globals.h"
#include "../engine/renderer.h"
#include "../engine/material.h"

#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <memory>

MyApp::MyApp() :
    camera(nullptr),
    input(nullptr),
    renderSettings(nullptr),
    offset(nullptr),
    bar(nullptr),
    monkey1(nullptr),
    monkey2(nullptr),
    monkey3(nullptr),
    icosphere(nullptr),
    light1(nullptr),
    light2(nullptr),
    light3(nullptr),
    dirLight(nullptr),
    rotationAxis(0.0f, 1.0f, 0.0f),
    rotationSpeed(0.5f),
    fps(0),
    fpsCounter(0),
    drawCount(&moar::DRAW_COUNT),
    timeCounter(0.0)
{
}

MyApp::~MyApp()
{
    TwDeleteBar(bar);
}

void MyApp::start()
{
    camera = engine->getCamera();
    input = engine->getInput();
    renderSettings = engine->getRenderSettings();

    moar::Object* plane = createRenderObject("diffuse", "plane.3ds", "white.png");
    plane->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    plane->setScale(glm::vec3(10.0f, 1.0f, 10.0f));
    plane->setName("plane");

    monkey1 = createRenderObject("diffuse", "monkey.3ds", "checker.png");
    monkey1->setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
    monkey1->setName("diffuse_monkey");

    monkey2 = createRenderObject("specular", "monkey.3ds", "checker.png");
    monkey2->setPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    monkey2->setName("specular_monkey");
    moar::Material* mat = monkey2->getComponent<moar::Material>();
    mat->setSpecularity(50.0f);

    monkey3 = createRenderObject("diffuse", "monkey.3ds", "checker.png");
    monkey3->setPosition(glm::vec3(0.0f, 3.0f, -10.0f));
    monkey3->setRotation(glm::vec3(-0.7f, 3.14f, 0.0f));
    monkey3->setScale(glm::vec3(0.3f, 0.3f, 0.3f));

    icosphere = createRenderObject("normalmap", "icosphere.3ds", "brick.png");
    icosphere->setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
    icosphere->setName("icosphere");
    mat = icosphere->getComponent<moar::Material>();
    mat->setTexture(engine->getResourceManager()->getTexture("brick_nmap.png"), moar::Material::TextureType::NORMAL, GL_TEXTURE_2D);

    // Todo: Easy way to visualize light type, position and direction.

    light1 = createLight(glm::vec4(0.0f, 1.0f, 0.0f, 3.0f));
    light1->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
    light2 = createLight(glm::vec4(1.0f, 0.0f, 0.0f, 4.0f));
    light2->setPosition(glm::vec3(0.0f, -3.0f, 0.0f));
//    light3 = createLight(glm::vec4(0.0f, 0.0f, 1.0f, 5.0f));
//    light3->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    dirLight = createLight(glm::vec4(1.0f, 1.0f, 1.0f, 1.2f), moar::Light::DIRECTIONAL);
    dirLight->setPosition(glm::vec3(0.0f, 3.0f, -10.0f));
    dirLight->setRotation(glm::vec3(-0.7f, 3.14f, 0.0f));
//    offset = camera->addPostprocess("offset", engine->getResourceManager()->getShader("offset"), 1);
//    offset->setUniform("screensize", std::bind(glUniform2f, moar::SCREEN_SIZE_LOCATION, renderSettings->windowWidth, renderSettings->windowHeight));
//    camera->addPostprocess("invert", engine->getResourceManager()->getShader("invert"), 1);

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

    monkey1->rotate(rotationAxis, std::fabs(sin(time)) * rotationSpeed * boost::math::constants::degree<double>());
    monkey2->rotate(rotationAxis, std::fabs(sin(time)) * rotationSpeed * boost::math::constants::degree<double>());

    light1->move(glm::vec3(0.0f, sin(time) * 0.1f, 0.0f));
    light2->move(glm::vec3(0.0f, cos(time) * 0.1f, 0.0f));

//    offset->setUniform("time", std::bind(glUniform1f, moar::TIME_LOCATION, glfwGetTime()));
}

void MyApp::initGUI()
{
    bar = TwNewBar("TweakBar");
    TwAddVarRW(bar, "axis", TW_TYPE_DIR3F, &rotationAxis, "");
    TwAddVarRW(bar, "speed", TW_TYPE_FLOAT, &rotationSpeed, "");
    TwAddVarRO(bar, "fps", TW_TYPE_INT32, &fps, "");
    TwAddVarRO(bar, "draw count", TW_TYPE_UINT32, drawCount, "");
}

moar::Object* MyApp::createRenderObject(const std::string& shader, const std::string& modelName, const std::string& textureName)
{
    GLuint texture = 0;
    if (!textureName.empty()) {
        texture = engine->getResourceManager()->getTexture(textureName);
    }
    std::shared_ptr<moar::Material> material(new moar::Material());
    material->setShaderType(shader);
    material->setTexture(texture, moar::Material::TextureType::DIFFUSE, GL_TEXTURE_2D);

    std::shared_ptr<moar::Renderer> renderer(new moar::Renderer());
    moar::Model* model = engine->getResourceManager()->getModel(modelName);
    renderer->setModel(model);

    std::shared_ptr<moar::Object> renderObj(new moar::Object());
    renderObj->addComponent(material);
    renderObj->addComponent(renderer);
    engine->addObject(renderObj);
    return renderObj.get();
}

moar::Object* MyApp::createLight(const glm::vec4& color, moar::Light::Type type)
{
    std::shared_ptr<moar::Light> lightComponent(new moar::Light(type));
    lightComponent->setColor(color);
    std::shared_ptr<moar::Object> light(new moar::Object());
    light->addComponent(lightComponent);
    engine->addObject(light);
    return light.get();
}
