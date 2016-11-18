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
    levelInfos = {
        {"droid.lvl", std::vector<glm::vec3>{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-2.0f, 1.0f, 1.0f)}, 0},
        {"sponza.lvl",std::vector<glm::vec3>{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-2.0f, 1.0f, 1.0f)}, 0}
    };
    currentLevelInfo = &levelInfos[0];
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
    if (!engine->loadLevel(currentLevelInfo->filename)) {
        std::cerr << "WARNING: Level loading failed\n";
    }

//#define POSTPROC
#define HDR_BLOOM
//#define GUI

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
}

void MyApp::handleInput(GLFWwindow* window)
{
    if (input->isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (input->isKeyPressed(GLFW_KEY_ESCAPE)) {
        quit();
    }

    if (input->isKeyDown(GLFW_KEY_W)) {
        camera->move(camera->getForward() * input->getMovementSpeed() * time->getDelta());
    }
    if (input->isKeyDown(GLFW_KEY_S)) {
        camera->move(-camera->getForward() * input->getMovementSpeed() * time->getDelta());
    }
    if (input->isKeyDown(GLFW_KEY_A)) {
        camera->move(camera->getLeft() * input->getMovementSpeed() * time->getDelta());
    }
    if (input->isKeyDown(GLFW_KEY_D)) {
        camera->move(-camera->getLeft() * input->getMovementSpeed() * time->getDelta());
    }

    if (input->isKeyPressed(GLFW_KEY_B)) {
        int iterations = camera->getBloomIterations() + 4;
        iterations = iterations > 30 ? 0 : iterations;
        camera->setBloomIterations(iterations);
    }
    if (input->isKeyPressed(GLFW_KEY_H)) {
        camera->setHDREnabled(!camera->isHDREnabled());
    }

    if (input->isKeyPressed(GLFW_KEY_R)) {
        unsigned int& i = currentLevelInfo->positionIndex;
        i = i == currentLevelInfo->cameraPositions.size() - 1 ? 0 : i + 1;
        camera->setPosition(currentLevelInfo->cameraPositions[i]);
    }

    camera->rotate(moar::Object::UP, -input->getCursorDeltaX() * boost::math::constants::degree<double>());
    camera->rotate(moar::Object::LEFT, input->getCursorDeltaY() * boost::math::constants::degree<double>());

    bool loadLevel = true;
    try {
        if (input->isKeyPressed(GLFW_KEY_1)) {
            currentLevelInfo = &levelInfos.at(0);
        } else if (input->isKeyPressed(GLFW_KEY_2)) {
            currentLevelInfo = &levelInfos.at(1);
        } else {
            loadLevel = false;
        }
    } catch (std::exception& e) {
        std::cerr << "WARNING: Invalid level\n";
        loadLevel = false;
    }

    if (loadLevel) {
        if (!engine->loadLevel(currentLevelInfo->filename)) {
            std::cerr << "WARNING: Level loading failed\n";
        }
    }
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
