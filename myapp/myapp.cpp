#include "myapp.h"
#include "../engine/renderer.h"
#include "../engine/material.h"

#include <boost/math/constants/constants.hpp>

MyApp::MyApp()
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
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
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

void MyApp::update(double)
{
    torus1->rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.0001f);
    torus2->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.001f);
    monkey1->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.001f);
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
    TwAddVarRW(bar, "speed", TW_TYPE_DOUBLE, &speed, "");
    TwAddVarRW(bar, "wire", TW_TYPE_BOOL32, &wire, "");
    TwAddVarRO(bar, "time", TW_TYPE_DOUBLE, &time, "");
    TwAddVarRW(bar, "bgColor", TW_TYPE_COLOR3F, &bgColor, "");
    TwAddVarRW(bar, "cubeColor", TW_TYPE_COLOR32, &cubeColor, "");
}
