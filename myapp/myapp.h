#ifndef MYAPP_H
#define MYAPP_H

#include "../engine/application.h"
#include "../engine/engine.h"

class MyApp : public moar::Application
{
public:
    MyApp();
    virtual ~MyApp() final;

    virtual void start() final;
    virtual void handleInput(GLFWwindow* window) final;
    virtual void update(double time) final;

private:
    moar::Object* createRenderObject(const std::string& shaderName, const std::string& modelName, const std::string& textureName);

    moar::Engine* engine;
    moar::Camera* camera;
    moar::Input* input;
    moar::Object* monkey1;
    moar::Object* torus1;
    moar::Object* torus2;
    moar::Object* ico;
};

#endif // MYAPP_H
