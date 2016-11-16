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
    input = engine->getInput();
    renderSettings = engine->getRenderSettings();
    time = engine->getTime();
    if (!engine->loadLevel(engine->getResourceManager()->getLevelPath() + "droid.lvl")) {
        std::cerr << "WARNING: Level loading failed\n";
    }

//#define MONKEY
//#define MOVE
//#define DIR_LIGHT
//#define POSTPROC
#define HDR_BLOOM
//#define GUI

#ifdef MONKEY
    monkey = engine->createObject();
    monkey->setShadowReceiver(false);
    monkey->addComponent<moar::Model>(engine->getResourceManager()->getModel("monkey.3ds"));
    moar::Material* m = engine->getResourceManager()->createMaterial();
    m->setTexture(engine->getResourceManager()->getTexture("spnza_bricks_a_diff.tga"), moar::Material::DIFFUSE, GL_TEXTURE_2D);
    m->setTexture(engine->getResourceManager()->getTexture("spnza_bricks_a_spec.tga"), moar::Material::SPECULAR, GL_TEXTURE_2D);
//    m->setTexture(engine->getResourceManager()->getTexture("brickwork_nmap.png"), moar::Material::NORMAL, GL_TEXTURE_2D);
//    m->setTexture(engine->getResourceManager()->getTexture("brickwork_bmap.png"), moar::Material::BUMP, GL_TEXTURE_2D);
    int shaderType =
            moar::Shader::DIFFUSE |
            moar::Shader::SPECULAR;
//            moar::Shader::NORMAL |
//            moar::Shader::BUMP;
    m->setShaderType(shaderType);
    for (auto& mo : monkey->getMeshObjects()) {
        mo.material = m;
    }
    monkey->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    monkey->setPosition(glm::vec3(-2.0f, 0.5f, 0.0f));
#endif

#ifdef POSTPROC
    offset = camera->addPostprocess("offset", engine->getResourceManager()->getShader("offset")->getProgram(), 1);
    offset->setUniform("screensize", std::bind(glUniform2f, moar::SCREEN_SIZE_LOCATION, renderSettings->windowWidth, renderSettings->windowHeight));
    camera->addPostprocess("invert", engine->getResourceManager()->getShader("invert")->getProgram(), 1);
#endif

#ifdef HDR_BLOOM
    camera->setHDREnabled(true);
    camera->setBloomIterations(12);
#endif

    initGUI();
    resetCamera(1);
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
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera->setBloomIterations(0);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        camera->setBloomIterations(12);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        camera->setHDREnabled(false);
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        camera->setHDREnabled(true);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        resetCamera(1);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        resetCamera(2);
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
#ifdef MOVE
    float t = time->getTime();
    float sint = static_cast<float>(sin(t));
    light1->setPosition(glm::vec3(1.5 + sin(t * 0.1), 1.8 + (sint * 0.3), 0.0f));
    light2->setPosition(glm::vec3(1.5 - sin(t * 0.2), 1.7 + (sint * 0.5), 0.0f));
    light3->setPosition(glm::vec3(-1.5 - sin(t * 0.1), 1.8 + (sint * 0.4), 0.0f));
    light4->setPosition(glm::vec3(-1.5 + sin(t * 0.3), 1.7 + (sint * 0.3), 0.0f));
    light5->move(glm::vec3(0.0f, sin(1.5f * t) * 0.01f, sint * 0.03f));
#endif
    position = camera->getPosition();

#ifdef POSTPROC
    offset->setUniform("time", std::bind(glUniform1f, moar::TIME_LOCATION, glfwGetTime()));
#endif
}

void MyApp::initGUI()
{
#ifdef GUI
    bar = TwNewBar("GUI");
    TwAddVarRO(bar, "fps", TW_TYPE_INT32, &fps, "");
    TwAddVarRO(bar, "draw count", TW_TYPE_UINT32, drawCount, "");
    TwAddVarRO(bar, "position", TW_TYPE_DIR3F, &position, "");
#endif
}

void MyApp::resetCamera(int spot)
{
    switch(spot) {
    case 1: {
        camera->setPosition(glm::vec3(0.0f, 0.5f, 0.5f));
        camera->setRotation(glm::vec3(0.0f, 0.9f, 0.0f));
        break;
    }
    case 2: {
        camera->setPosition(glm::vec3(1.8f, 0.9f, -1.5f));
        camera->setRotation(glm::vec3(0.0f, -1.2f, 0.0f));
        break;
    }
    default: {
    }
    }
}
