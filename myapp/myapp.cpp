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
         std::vector<glm::vec3>{
             glm::vec3(1.0f, 1.0f, 1.0f),
             glm::vec3(0.3f, 0.9f, 1.3f),
             glm::vec3(0.58f, 0.37f, -0.3f)},
         std::vector<glm::vec3>{
             glm::vec3(-0.5f, -5.5f, 0.0f),
             glm::vec3(-0.7f, -5.8f, 0.0f),
             glm::vec3(-0.1f, -4.2f, 0.0f)},
         0},
        {"sponza.lvl",
         std::vector<glm::vec3>{
             glm::vec3(-0.3f, 2.2f, 0.4f),
             glm::vec3(0.0f, 0.4f, -0.8f),
             glm::vec3(-5.3f, 0.9f, -0.5f)},
         std::vector<glm::vec3>{
             glm::vec3(-0.5f, -4.9f, 0.0f),
             glm::vec3(0.3f, -4.4f, 0.0f),
             glm::vec3(-0.3f, -1.6f, 0.0f)},
         0}
    };
    if (levelInfos.empty()) {
        std::cerr << "WARNING: Empty level infos\n";
    } else {
        currentLevelInfo = &levelInfos[1];
    }
    for (const auto& info : levelInfos) {
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

    engine->setDeferredRendering(deferred);
    camera->setBloomIterations(bloomIterations);
    camera->setHDREnabled(HDR);
    camera->setSSAOEnabled(SSAO);
    camera->setFXAAEnabled(FXAA);

//#define POSTPROC
#ifdef POSTPROC
    offset = camera->addPostprocess("offset", engine->getResourceManager()->getShaderByName("offset")->getProgram(), 1);
    offset->setUniform("screensize", std::bind(glUniform2f, moar::SCREEN_SIZE_LOCATION, renderSettings->windowWidth, renderSettings->windowHeight));
    camera->addPostprocess("invert", engine->getResourceManager()->getShaderByName("invert")->getProgram(), 1);
#endif
}

void MyApp::levelLoaded()
{
    if (currentLevelInfo) {
        currentLevelInfo->positionIndex = 0;
        resetCamera();
		if (currentLevelInfo->filename == "sponza.lvl") {
            light0 = engine->getObjectByName("light0");
			light1 = engine->getObjectByName("light1");
			light2 = engine->getObjectByName("light2");
		}
		else {
            light0 = nullptr;
			light1 = nullptr;
			light2 = nullptr;
		}
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

    if (input->isKeyPressed(GLFW_KEY_F)) {
        deferred = !deferred;
        engine->setDeferredRendering(deferred);
    }
    if (input->isKeyPressed(GLFW_KEY_B)) {
        bloomIterations = camera->getBloomIterations() + 4;
        bloomIterations = bloomIterations > 30 ? 0 : bloomIterations;
        camera->setBloomIterations(bloomIterations);
    }
    if (input->isKeyPressed(GLFW_KEY_H)) {
        HDR = !HDR;
        camera->setHDREnabled(HDR);
    }
    if (input->isKeyPressed(GLFW_KEY_J)) {
        SSAO = !SSAO;
        camera->setSSAOEnabled(SSAO);
    }
    if (input->isKeyPressed(GLFW_KEY_G)) {
        FXAA = !FXAA;
        camera->setFXAAEnabled(FXAA);
    }
    if (input->isKeyPressed(GLFW_KEY_R)) {
        resetCamera();
    }
	
    camera->rotate(moar::Object::UP, -input->getCursorDeltaX() * boost::math::constants::degree<float>());
    camera->rotate(moar::Object::LEFT, input->getCursorDeltaY() * boost::math::constants::degree<float>());

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
        std::cerr << "WARNING: Invalid level. " << e.what() << "\n";
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
    moar::Object* light = engine->getObjectByName("light4");
    if (light) {
        light->rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.1f * time->getDelta());
    }

	performanceData = engine->getPerformanceData();
    camPos = camera->getPosition();
    camRot = camera->getRotation();

    if (light0 && light1 && light2) {
        float t = time->getTime();
        float sint = static_cast<float>(sin(t));
        float cost = static_cast<float>(cos(t));
        light0->setPosition(glm::vec3(-4.0f, 1.0 + (sint * 0.3), 0.0f));
        light1->setPosition(glm::vec3(-3.0f, 1.5 + (cost * 0.3), 0.0f));
        light2->setPosition(glm::vec3(-2.0f, 1.8 + (sint * 0.3), 0.0f));
    }

#ifdef POSTPROC
    offset->setUniform("time", std::bind(glUniform1f, moar::TIME_LOCATION, glfwGetTime()));
#endif
}

void MyApp::initGUI()
{
    bar = TwNewBar("GUI");
    TwDefine(" GUI size='300 200' ");
    TwDefine(" GUI valueswidth=140 ");
    TwDefine(" GUI refresh=0.5 ");
    TwAddVarRO(bar, "FPS", TW_TYPE_INT32, &performanceData.FPS, "");
    TwAddVarRO(bar, "Draw count", TW_TYPE_UINT32, &performanceData.drawCount, "");
	TwAddVarRO(bar, "GPU Idle time %", TW_TYPE_FLOAT, &performanceData.gpuIdle, "");
    TwAddVarRO(bar, "Deferred", TW_TYPE_BOOLCPP, &deferred, "");    
    TwAddVarRO(bar, "Bloom", TW_TYPE_UINT32, &bloomIterations, "");
    TwAddVarRO(bar, "HDR", TW_TYPE_BOOLCPP, &HDR, "");
    TwAddVarRO(bar, "SSAO", TW_TYPE_BOOLCPP, &SSAO, "");
    TwAddVarRO(bar, "FXAA", TW_TYPE_BOOLCPP, &FXAA, "");
	TwAddVarRO(bar, "Camera position", TW_TYPE_DIR3F, &camPos, "");
	TwAddVarRO(bar, "Camera rotation", TW_TYPE_DIR3F, &camRot, "");
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
