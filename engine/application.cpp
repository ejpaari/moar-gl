#include "application.h"
#include "engine.h"

#include <boost/math/constants/constants.hpp>
#include <iostream>

namespace moar
{

Application::Application() :
    engine(nullptr),
    running(true)
{
    // Todo: when using multiple cameras, this should be moved
    Renderer::view = camera.getViewMatrixPointer();
    Renderer::projection = camera.getProjectionMatrixPointer();
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
        renderObjects[i]->execute();
    }
}

Object* Application::createRenderObject(const std::string& shaderName, const std::string& modelName, const std::string& textureName)
{
    // Todo: Maybe this should be moved for the custom application
    GLuint shader = engine->getResourceManager()->getShader(shaderName);
    GLuint texture = 0;
    if (!textureName.empty()) {
        texture = engine->getResourceManager()->getTexture(textureName);
    }
    Material* material = new Material();
    material->setShader(shader);
    material->setTexture(texture, Material::TextureType::DIFFUSE);

    Renderer* renderer = new Renderer();
    Model* model = engine->getResourceManager()->getModel(modelName);
    renderer->setModel(model);

    std::shared_ptr<Object> renderObj(new Object());
    renderObj->setComponent(material);
    renderObj->setComponent(renderer);
    renderObjects.push_back(renderObj);
    return renderObj.get();
}

} // moar
