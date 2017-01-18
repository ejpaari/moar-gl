#ifndef MYAPP_H
#define MYAPP_H

#include "../engine/application.h"
#include "../engine/engine.h"
#include "../engine/input.h"
#include "../engine/camera.h"
#include "../engine/object.h"
#include "../engine/rendersettings.h"
#include "../engine/time.h"
#include "../engine/light.h"

#include <string>

class MyApp : public moar::Application
{
public:
    MyApp();
    virtual ~MyApp() final;

    virtual void start() final;
    virtual void levelLoaded() final;
    virtual void handleInput(GLFWwindow* window) final;
    virtual void update() final;

private:
    struct LevelInfo {
        std::string filename;
        std::vector<glm::vec3> cameraPositions;
        std::vector<glm::vec3> cameraRotations;
        unsigned int positionIndex;
    };

    void initGUI();
    void resetCamera();

    moar::Camera* camera = nullptr;
    moar::Input* input = nullptr;
    moar::RenderSettings* renderSettings = nullptr;
    moar::Time* time = nullptr;
    moar::Postprocess* offset = nullptr;

    moar::Object* light1 = nullptr;
    moar::Object* light2 = nullptr;

    TwBar* bar = nullptr;
    
    std::vector<LevelInfo> levelInfos;
    LevelInfo* currentLevelInfo = nullptr;

    unsigned int FPS = 0;
    unsigned int drawCount = 0;
    bool deferred = true;
    glm::vec3 position;
    glm::vec3 rotation;

    int bloomIterations = 0;
    bool HDR = false;
    bool SSAO = true;
};

#endif // MYAPP_H
