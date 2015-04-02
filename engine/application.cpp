#include "application.h"
#include "engine.h"
#include "model.h"

#include <boost/math/constants/constants.hpp>
#include <iostream>

namespace moar
{

Application::Application() :
    engine(nullptr),
    running(true)
{
    RenderObject::view = camera.getViewMatrixPointer();
    RenderObject::projection = camera.getProjectionMatrixPointer();
}

Application::~Application()
{
}

void Application::handleInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        quit();
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.move(camera.getForward() * input.getMovementSpeed());
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.move(-camera.getForward() * input.getMovementSpeed());
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.move(camera.getLeft() * input.getMovementSpeed());
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.move(-camera.getLeft() * input.getMovementSpeed());
    }

    camera.rotate(glm::vec3(0.0f, 1.0f, 0.0f), -input.getCursorDeltaX() * boost::math::constants::degree<double>());
    camera.rotate(glm::vec3(1.0f, 0.0f, 0.0f), input.getCursorDeltaY() * boost::math::constants::degree<double>());
}

void Application::render()
{
    for (unsigned int i = 0; i < renderObjects.size(); ++i) {
        renderObjects[i]->render();
    }
}

RenderObject* Application::createRenderObject(const std::string& shaderName, const std::string& modelName)
{
    GLuint shader = engine->getResourceManager()->getShader(shaderName);
    Model* model = engine->getResourceManager()->getModel(modelName);

    std::shared_ptr<RenderObject> renderObj(new RenderObject());
    renderObj->init(shader, model);
    renderObjects.push_back(renderObj);
    return renderObj.get();
}

} // moar
