#include "myapp.h"
#include "../engine/renderer.h"
#include "../engine/material.h"

#include <boost/math/constants/constants.hpp>

MyApp::MyApp() :
    rotationAxis(0.0f, 1.0f, 0.0f),
    rotationSpeed(0.0001f),
    fps(0),
    fpsCounter(0),
    timeCounter(0.0)
{
}

MyApp::~MyApp()
{
}

void MyApp::start()
{
    engine = getEngine();
    camera = engine->getCamera();
    input = engine->getInput();

    torus1 = createRenderObject("textured_normals", "torus.3ds", "brick.png");

    torus1->setPosition(glm::vec3(3.0f, 0.0f, 3.0f));
    moar::Material* mat = torus1->getComponent<moar::Material>();
    mat->setTexture(getEngine()->getResourceManager()->getTexture("brick_nmap.png"), moar::Material::TextureType::NORMAL);

    torus1->getComponent<moar::Material>();

    monkey1 = createRenderObject("textured_normals", "monkey.3ds", "checker.png");
    monkey1->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    torus2 = createRenderObject("normals", "torus.3ds", "");
    torus2->setPosition(glm::vec3(-1.0f, 0.0f, 3.0f));
    ico = createRenderObject("textured_normals", "icosphere.3ds", "marble.jpg");
    ico->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));

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

    camera->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -input->getCursorDeltaX() * boost::math::constants::degree<double>());
    camera->rotate(glm::vec3(1.0f, 0.0f, 0.0f), input->getCursorDeltaY() * boost::math::constants::degree<double>());
}

void MyApp::update(double, double deltaTime)
{
    timeCounter += deltaTime;
    if (timeCounter < 1.0) {
        ++fpsCounter;
    } else {
        fps = fpsCounter;
        timeCounter = 0.0;
        fpsCounter = 0;
    }

    torus1->rotate(rotationAxis, rotationSpeed * boost::math::constants::degree<double>());
    torus2->rotate(rotationAxis, rotationSpeed * boost::math::constants::degree<double>());
    monkey1->rotate(rotationAxis, rotationSpeed * boost::math::constants::degree<double>());
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
    material->setTexture(texture, moar::Material::TextureType::DIFFUSE);

    moar::Renderer* renderer = new moar::Renderer();
    moar::Model* model = engine->getResourceManager()->getModel(modelName);
    renderer->setModel(model);

    moar::Object* renderObj = new moar::Object();
    renderObj->setComponent(material);
    renderObj->setComponent(renderer);
    engine->addRenderObject(renderObj);
    return renderObj;
}

void MyApp::initGUI()
{
    bar = TwNewBar("TweakBar");
    TwAddVarRW(bar, "axis", TW_TYPE_DIR3F, &rotationAxis, "");
    TwAddVarRW(bar, "speed", TW_TYPE_FLOAT, &rotationSpeed, "");
    TwAddVarRO(bar, "fps", TW_TYPE_INT32, &fps, "");
}
