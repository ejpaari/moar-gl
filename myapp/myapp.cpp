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
    monkey = createRenderObject("test", "monkey.3ds");
    torus = createRenderObject("test", "torus.3ds");
}

void MyApp::update(double time)
{
    if (monkey != nullptr) {
        monkey->rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.1f);
        monkey->setPosition(glm::vec3(0.0f, std::sin(time), 0.0f));
    }
}
