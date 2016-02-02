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
    offset(nullptr),
    bar(nullptr),
    monkey(nullptr),
    icosphere(nullptr),
    light1(nullptr),
    light2(nullptr),
    dirLight(nullptr),
    fps(0),
    fpsCounter(0),
    drawCount(&moar::DRAW_COUNT),
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

    moar::Object* sponza = createRenderObject("diffuse", "sponza.3ds", "white.png");
    sponza->setScale(glm::vec3(3.0f, 3.0f, 3.0f));

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

    light1 = createLight(glm::vec4(0.8f, 1.0f, 0.8f, 1.0f));
    light1->setPosition(glm::vec3(1.0f, 1.5f, 0.0f));
    light2 = createLight(glm::vec4(1.0f, 1.0f, 0.8f, 1.0f));
    light2->setPosition(glm::vec3(-1.0f, 1.5f, 0.0f));
    //    light2->getComponent<moar::Light>()->setShadowingEnabled(false);
    //    dirLight = createLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.1f), moar::Light::DIRECTIONAL);
    //    dirLight->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    //    dirLight->setRotation(glm::vec3(-1.0f, 0.0f, 0.0f));
    //    offset = camera->addPostprocess("offset", engine->getResourceManager()->getShader("offset")->getProgram(), 1);
    //    offset->setUniform("screensize", std::bind(glUniform2f, moar::SCREEN_SIZE_LOCATION, renderSettings->windowWidth, renderSettings->windowHeight));
    //    camera->addPostprocess("invert", engine->getResourceManager()->getShader("invert")->getProgram(), 1);

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
        resetCamera();
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

    //    monkey1->rotate(rotationAxis, rotationSpeed * boost::math::constants::degree<double>());

    light1->move(glm::vec3(0.0f, sin(time) * 0.01f, 0.0f));
    light2->move(glm::vec3(0.0f, cos(time) * 0.02f, 0.0f));

    //    offset->setUniform("time", std::bind(glUniform1f, moar::TIME_LOCATION, glfwGetTime()));
}

void MyApp::initGUI()
{
    bar = TwNewBar("GUI");
    TwAddVarRO(bar, "fps", TW_TYPE_INT32, &fps, "");
    TwAddVarRO(bar, "draw count", TW_TYPE_UINT32, drawCount, "");
}

moar::Object* MyApp::createRenderObject(const std::string& /*shader*/, const std::string& modelName, const std::string& /*textureName*/)
{
    moar::Object* renderObj= engine->createObject();
    moar::Model* model = engine->getResourceManager()->getModel(modelName);
    renderObj->setModel(model);

//    moar::Material* material = engine->getResourceManager()->createMaterial();
//    renderObj->setMeshMaterial(material);
//    GLuint texture = 0;
//    if (!textureName.empty()) {
//        texture = engine->getResourceManager()->getTexture(textureName);
//    }
//    material->setShaderType(shader);
//    material->setTexture(texture, moar::Material::TextureType::DIFFUSE, GL_TEXTURE_2D);
//    if (shader == "diffuse") {
//        material->setUniform("solidColor",
//                             std::bind(glUniform3f, moar::SOLID_COLOR_LOCATION, 1.0f, 1.0f, 1.0f),
//                             moar::SOLID_COLOR_LOCATION);
//    }

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
