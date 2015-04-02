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
    glDeleteTextures(1, &textures[0]);
}

bool RenderObject::init(GLuint shaderProgram, Model* renderModel)
{
    shader = shaderProgram;
    glUseProgram(shader);

    model = renderModel;

    // Todo: Texture
    glGenTextures(1, textures);

    int width, height;
    unsigned char* image;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    image = SOIL_load_image("../moar-gl/textures/checker.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shader, "myTexture"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void RenderObject::render()
{
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(getModelMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(*view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "proj"), 1, GL_FALSE, glm::value_ptr(*projection));
    model->render();
}

} // moar

