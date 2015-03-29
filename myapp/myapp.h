#ifndef MYAPP_H
#define MYAPP_H

#include "../engine/application.h"


class MyApp : public moar::Application {
public:
    MyApp();
    virtual ~MyApp() final;

    void virtual start() final;
    void virtual update() final;

private:
    moar::RenderObject monkey;
    moar::RenderObject torus;
};

#endif // MYAPP_H
