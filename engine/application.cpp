#include "application.h"

#include <iostream>

namespace moar
{

Application::Application() :
    running(true),
    movementSpeed(0.01f)
{
    RenderObject::view = camera.getViewMatrixPointer();
    RenderObject::projection = camera.getProjectionMatrixPointer();
    camera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
}

Application::~Application()
{
}

void Application::input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        quit();
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.move(camera.getForward() * movementSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.move(-camera.getForward() * movementSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.move(camera.getLeft() * movementSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.move(-camera.getLeft() * movementSpeed);
    }

    double x = 0.0;
    double y = 0.0;
    glfwGetCursorPos(window, &x, &y);
    mouse.setPosition(x, y);

    camera.rotate(glm::vec3(0.0f, 1.0f, 0.0f), -mouse.getDeltaX());
    camera.rotate(glm::vec3(1.0f, 0.0f, 0.0f), mouse.getDeltaY());
}

void Application::render()
{
    for (unsigned int i = 0; i < renderObjects.size(); ++i) {
        renderObjects[i]->render();
    }
}

RenderObject* Application::createRenderObject(const std::string& shaderName, const std::string& modelName)
{
    GLuint shaderProgram;
    auto found = shaders.find(shaderName);
    if (found == shaders.end()) {
        std::unique_ptr<Shader> shader(new Shader());
        std::string path = "../moar-gl/myapp/shaders/";
        path += shaderName;
        std::string vertexShaderFile = path;
        vertexShaderFile  += ".vert";
        std::string fragmentShaderFile = path;
        fragmentShaderFile += ".frag";

        bool isGood;
        isGood = shader->attachShader(GL_VERTEX_SHADER, vertexShaderFile.c_str());
        if (!isGood) {
            std::cerr << "Failed to attach vertex shader: " << shaderName << std::endl;
            return nullptr;
        }

        isGood = shader->attachShader(GL_FRAGMENT_SHADER, fragmentShaderFile.c_str());
        if (!isGood) {
            std::cerr << "Failed to attach fragment shader: " << shaderName << std::endl;
            return nullptr;
        }

        shader->linkProgram();
        shaderProgram = shader->getProgram();
        shaders.insert(std::pair<std::string, std::unique_ptr<Shader>>(shaderName, std::move(shader)));
    } else {
        shaderProgram = found->second->getProgram();
    }

    std::shared_ptr<RenderObject> renderObj(new RenderObject());
    renderObj->init(shaderProgram, modelName);
    renderObjects.push_back(renderObj);
    return renderObj.get();
}

} // moar
