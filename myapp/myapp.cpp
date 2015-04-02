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
//    monkey1 = createRenderObject("textured_normals", "monkey.3ds");
//    monkey1->setPosition(glm::vec3(3.0f, 0.0f, 0.0f));
//    monkey2 = createRenderObject("normals", "monkey.3ds");
//    monkey2->setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
//    torus1 = createRenderObject("textured_normals", "torus.3ds");
//    torus1->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
//    torus2 = createRenderObject("normals", "torus.3ds");
//    torus2->setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
//    monkey3 = createRenderObject("textured_normals", "monkey.3ds");
//    monkey3->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
//    torus3 = createRenderObject("textured_normals", "torus.3ds");
//    torus3->setPosition(glm::vec3(0.0f, -3.0f, 0.0f));

    torus1 = createRenderObject("textured_normals", "torus.3ds");
    torus1->setPosition(glm::vec3(1.0f, 0.0f, 3.0f));
    monkey1 = createRenderObject("textured_normals", "monkey.3ds");
    monkey1->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    torus2 = createRenderObject("normals", "torus.3ds");
    torus2->setPosition(glm::vec3(-1.0f, 0.0f, 3.0f));
}

void MyApp::update(double)
{
    torus1->rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.01f);
    torus2->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.01f);
    monkey1->rotate(glm::vec3(0.0f, 1.0f, 0.0f), -0.01f);
}
