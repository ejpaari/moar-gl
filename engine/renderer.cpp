#include "renderer.h"
#include "object.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <SOIL.h>
#include <iostream>

namespace moar
{

const glm::mat4* Renderer::projection = nullptr;
const glm::mat4* Renderer::view = nullptr;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::execute()
{
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
    glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, glm::value_ptr(parent->getModelMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(currentShader, "view"), 1, GL_FALSE, glm::value_ptr(*view));
    glUniformMatrix4fv(glGetUniformLocation(currentShader, "proj"), 1, GL_FALSE, glm::value_ptr(*projection));
    model->render();
}

} // moar

