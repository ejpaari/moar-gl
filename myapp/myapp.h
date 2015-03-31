#ifndef MYAPP_H
#define MYAPP_H

#include "../engine/application.h"
#include "../engine/renderobject.h"

class MyApp : public moar::Application
{
public:
    MyApp();
    virtual ~MyApp() final;

    void virtual start() final;
    void virtual update(double time) final;

private:
    moar::RenderObject* monkey;
    moar::RenderObject* torus;
};

#endif // MYAPP_H
