#include "myapp.h"

MyApp::MyApp()
{
}

MyApp::~MyApp()
{
}

void MyApp::start()
{
    createRenderObject("test", "monkey.3ds");
    createRenderObject("test", "torus.3ds");
}

void MyApp::update()
{
}
