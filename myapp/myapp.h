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
    moar::Object* createRenderObject(const std::string& modelName);
    moar::Object* createLight(const glm::vec4& color, moar::Light::Type type = moar::Light::POINT);
    void resetCamera();

    moar::Camera* camera = nullptr;
    moar::Input* input = nullptr;
    moar::RenderSettings* renderSettings = nullptr;
    moar::Time* time = nullptr;
    moar::Postprocess* offset = nullptr;
    TwBar* bar = nullptr;

    moar::Object* light1a = nullptr;
    moar::Object* light1b = nullptr;
    moar::Object* light2a = nullptr;
    moar::Object* light2b = nullptr;
    moar::Object* light3 = nullptr;
    moar::Object* monkey = nullptr;

    int fps = 0;
    int fpsCounter = 0;
    unsigned int* drawCount;
    glm::vec3 position;
    double timeCounter = 0.0;
};

#endif // MYAPP_H
