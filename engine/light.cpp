#include "light.h"
#include "object.h"
#include "common/globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace moar
{

GLuint Light::lightBlockBuffer = 0;
bool Light::bufferCreated = false;

Light::Light()
{
    if (!bufferCreated) {
        glGenBuffers(1, &lightBlockBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
        GLsizeiptr bufferSize = 16 + 16 + 16;
        glBufferData(GL_UNIFORM_BUFFER, bufferSize, 0, GL_DYNAMIC_DRAW); // Initialize as empty
        bufferCreated = true;
    }
}

Light::~Light()
{
    glDeleteBuffers(1, &lightBlockBuffer);
}

void Light::execute(const glm::vec3& position, const glm::vec3& forward)
{
    // Todo: sizeof is called each time! Call it once and store the value.
    glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
    GLintptr offset = 0;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(color), glm::value_ptr(color));
    offset += 16;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(position), glm::value_ptr(position));
    offset += 16;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(forward), glm::value_ptr(forward));
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING_POINT, lightBlockBuffer);
}

void Light::setType(Light::Type type)
{
    this->type = type;
    COMPONENT_CHANGED = true;
}

void Light::setShadowingEnabled(bool enabled)
{
    shadowingEnabled = enabled;
}

void Light::setColor(const glm::vec4& color)
{
    this->color = color;
}

bool Light::isShadowingEnabled() const
{
    return shadowingEnabled;
}

Light::Type Light::getLightType() const
{
    return type;
}

} // moar

