#include "myapp.h"

#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

MyApp::MyApp()
{
}

MyApp::~MyApp()
{
}

void MyApp::start()
{
    monkey1 = createRenderObject("test", "monkey.3ds");
    monkey1->setPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    monkey2 = createRenderObject("test", "monkey.3ds");
    monkey2->setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
    torus1 = createRenderObject("test", "torus.3ds");
    torus1->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    torus2 = createRenderObject("test", "torus.3ds");
    torus2->setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
}

void MyApp::update(double)
{
}
