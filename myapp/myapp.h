#ifndef MYAPP_H
#define MYAPP_H

#include "../engine/application.h"
#include "../engine/shader.h"
#include "../engine/model.h"

#include <glm/glm.hpp>

class MyApp : public moar::Application {
public:
    MyApp();
    virtual ~MyApp() final;

    void virtual initialize() final;
    void virtual run() final;

private:
    moar::Shader shader;
    moar::Model model;

    GLint uniView;
    GLint uniProj;

    GLuint textures[1];
};

#endif // MYAPP_H
