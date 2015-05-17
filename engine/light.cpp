#include "light.h"
#include "object.h"
#include "constants.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>

namespace moar
{

GLint Light::shader = 0;
GLuint Light::lightBlockBuffer = 0;
bool Light::lightBlockBufferCreated = false;

Light::Light() :
    color(1.0f, 1.0f, 1.0f),
    power(10.0f)
{
    if (!lightBlockBufferCreated) {
        glGenBuffers(1, &lightBlockBuffer);
        lightBlockBufferCreated = true;
    }
}

Light::~Light()
{
    glDeleteBuffers(1, &lightBlockBuffer);
}

void Light::execute()
{
    // Todo: Should every light have its own buffer?
    glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(color), glm::value_ptr(color), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING_POINT, lightBlockBuffer);

    // Todo: Add all light properties to uniform buffer.
    glGetIntegerv(GL_CURRENT_PROGRAM, &shader);
    glUniform3f(glGetUniformLocation(shader, "lightPos"), parent->getPosition().x, parent->getPosition().y, parent->getPosition().z);
    //glUniform3f(glGetUniformLocation(shader, "lightColor"), color.x, color.y, color.z);
    glUniform1f(glGetUniformLocation(shader, "lightPower"), power);
}

void Light::setColor(const glm::vec3& color)
{
    this->color = color;
}

void Light::setPower(float power)
{
    this->power = power;
}

std::string Light::getName()
{
    return "Light";
}

Component::Type Light::getType()
{
    return Component::Type::LIGHT;
}

} // moar

