#ifndef MYAPP_H
#define MYAPP_H

#include "../engine/application.h"
#include "../engine/shader.h"

class MyApp : public moar::Application {
public:
    MyApp();
    virtual ~MyApp() final;

    void virtual initialize() final;
    void virtual run() final;

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    moar::Shader shader;

    GLint posAttrib;
    GLint colAttrib;
    GLint texAttrib;

    GLuint textures[1];
};

#endif // MYAPP_H
