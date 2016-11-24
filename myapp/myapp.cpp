#include "myapp.h"
#include "../engine/model.h"
#include "../engine/material.h"

#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <memory>

MyApp::MyApp()
{
    levelInfos = {
        {"droid.lvl",
         std::vector<glm::vec3>{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.58f, 0.37f, -0.3f)},
         std::vector<glm::vec3>{glm::vec3(-0.5f, -5.5f, 0.0f), glm::vec3(-0.1f, -4.2f, 0.0f)},
         0},
        {"sponza.lvl",
         std::vector<glm::vec3>{glm::vec3(-0.3f, 2.2f, 0.4f), glm::vec3(0.0f, 0.4f, -0.8f)},
         std::vector<glm::vec3>{glm::vec3(-0.5f, -4.9f, 0.0f), glm::vec3(0.3f, -4.4f, 0.0f)},
         0}
    };
    if (levelInfos.empty()) {
        std::cerr << "WARNING: Empty level infos\n";
    } else {
        currentLevelInfo = &levelInfos[0];
    }
    for (auto& info : levelInfos) {
        if (info.cameraPositions.size() != info.cameraRotations.size()) {
            std::cerr << "WARNING: Level info camera positions and camera rotations sizes mismatch\n";
        }
    }
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

    initGUI();

    camera->setHDREnabled(true);
    camera->setBloomIterations(4);

//#define POSTPROC
#ifdef POSTPROC
    offset = camera->addPostprocess("offset", engine->getResourceManager()->getShader("offset")->getProgram(), 1);
    offset->setUniform("screensize", std::bind(glUniform2f, moar::SCREEN_SIZE_LOCATION, renderSettings->windowWidth, renderSettings->windowHeight));
    camera->addPostprocess("invert", engine->getResourceManager()->getShader("invert")->getProgram(), 1);
#endif
}

void MyApp::levelLoaded()
{
    if (currentLevelInfo) {
        currentLevelInfo->positionIndex = 0;
        resetCamera();
    }
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
        resetCamera();
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
    fps = static_cast<unsigned int>(1.0f / time->getDelta());
    drawCount = engine->getDrawCount();
    position = camera->getPosition();
    rotation = camera->getRotation();

#ifdef MOVE
    float t = time->getTime();
    float sint = static_cast<float>(sin(t));
    light1->setPosition(glm::vec3(1.5 + sin(t * 0.1), 1.8 + (sint * 0.3), 0.0f));
    light2->setPosition(glm::vec3(1.5 - sin(t * 0.2), 1.7 + (sint * 0.5), 0.0f));
    light3->setPosition(glm::vec3(-1.5 - sin(t * 0.1), 1.8 + (sint * 0.4), 0.0f));
    light4->setPosition(glm::vec3(-1.5 + sin(t * 0.3), 1.7 + (sint * 0.3), 0.0f));
    light5->move(glm::vec3(0.0f, sin(1.5f * t) * 0.01f, sint * 0.03f));
#endif

#ifdef POSTPROC
    offset->setUniform("time", std::bind(glUniform1f, moar::TIME_LOCATION, glfwGetTime()));
#endif
}

void MyApp::initGUI()
{
    bar = TwNewBar("GUI");
    TwDefine(" GUI size='300 100' ");
    TwDefine(" GUI valueswidth=140 ");
    TwDefine(" GUI refresh=0.5 ");
    TwAddVarRO(bar, "fps", TW_TYPE_INT32, &fps, "");
    TwAddVarRO(bar, "draw count", TW_TYPE_UINT32, &drawCount, "");
    TwAddVarRO(bar, "position", TW_TYPE_DIR3F, &position, "");
    TwAddVarRO(bar, "rotation", TW_TYPE_DIR3F, &rotation, "");
}

void MyApp::resetCamera()
{
    if (currentLevelInfo) {
        unsigned int index = currentLevelInfo->positionIndex;
        if (index >= currentLevelInfo->cameraPositions.size() || index >= currentLevelInfo->cameraRotations.size()) {
            index = 0;
        }
        camera->setPosition(currentLevelInfo->cameraPositions[index]);
        camera->setRotation(currentLevelInfo->cameraRotations[index]);
        currentLevelInfo->positionIndex = ++index;
    }
}
