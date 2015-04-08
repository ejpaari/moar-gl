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
    moar::RenderObject* monkey1;
    moar::RenderObject* torus1;
    moar::RenderObject* torus2;
    moar::RenderObject* ico;
};

#endif // MYAPP_H
