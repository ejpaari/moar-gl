#ifndef MYAPP_H
#define MYAPP_H

#include "../engine/application.h"
#include "../engine/object.h"
#include "../engine/engine.h"

class MyApp : public moar::Application
{
public:
    MyApp();
    virtual ~MyApp() final;

    void virtual start() final;
    void virtual update(double time) final;

private:
    moar::Object* monkey1;
    moar::Object* torus1;
    moar::Object* torus2;
    moar::Object* ico;
};

#endif // MYAPP_H
