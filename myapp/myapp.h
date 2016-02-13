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

    moar::Camera* camera;
    moar::Input* input;
    moar::RenderSettings* renderSettings;
    moar::Time* time;
    moar::Postprocess* offset;
    TwBar* bar;

    moar::Object* monkey;
    moar::Object* icosphere;

    moar::Object* light1;
    moar::Object* light2;
    moar::Object* dirLight;

    int fps;
    int fpsCounter;
    unsigned int* drawCount;
    double timeCounter;
};

#endif // MYAPP_H
