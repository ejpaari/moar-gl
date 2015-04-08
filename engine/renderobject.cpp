#include "renderobject.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <SOIL.h>
#include <iostream>

namespace moar
{

const glm::mat4* RenderObject::projection = nullptr;
const glm::mat4* RenderObject::view = nullptr;

RenderObject::RenderObject()
{
}

RenderObject::~RenderObject()
{
}

bool RenderObject::init(GLuint shaderProgram, Model* renderModel, GLuint textureName)
{
    glUseProgram(shader);
    shader = shaderProgram;    
    model = renderModel;
    texture = textureName;

    return true;
}

void RenderObject::render()
{
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(getModelMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(*view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, glm::value_ptr(*projection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "myTexture"), 0);
    model->render();
}

} // moar

