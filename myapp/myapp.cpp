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
    torus1 = createRenderObject("textured_normals", "torus.3ds", "checker.png");
    torus1->setPosition(glm::vec3(1.0f, 0.0f, 3.0f));
    monkey1 = createRenderObject("textured_normals", "monkey.3ds", "checker.png");
    monkey1->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    torus2 = createRenderObject("normals", "torus.3ds");
    torus2->setPosition(glm::vec3(-1.0f, 0.0f, 3.0f));
    ico = createRenderObject("textured_normals", "icosphere.3ds", "marble.jpg");
    ico->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
}

void MyApp::update(double)
{
    torus1->rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.01f);
    torus2->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.01f);
    monkey1->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.01f);
}
