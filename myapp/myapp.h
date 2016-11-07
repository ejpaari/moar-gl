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

#include <glm/glm.hpp>
#include <AntTweakBar.h>
#include <string>

class MyApp : public moar::Application
{
public:
    MyApp();
    virtual ~MyApp() final;

    virtual void start() final;
    virtual void handleInput(GLFWwindow* window) final;
    virtual void update() final;

private:
    void initGUI();
    void resetCamera(int spot);

    moar::Camera* camera = nullptr;
    moar::Input* input = nullptr;
    moar::RenderSettings* renderSettings = nullptr;
    moar::Time* time = nullptr;
    moar::Postprocess* offset = nullptr;
    TwBar* bar = nullptr;

    moar::Object* light1 = nullptr;
    moar::Object* light2 = nullptr;
    moar::Object* light3 = nullptr;
    moar::Object* light4 = nullptr;
    moar::Object* light5 = nullptr;
    moar::Object* monkey = nullptr;

    int fps = 0;
    int fpsCounter = 0;
    unsigned int* drawCount;
    glm::vec3 position;
    double timeCounter = 0.0;
};

#endif // MYAPP_H
