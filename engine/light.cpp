#include "light.h"
#include "object.h"
#include "globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace moar
{

Light::Light(Type type) :
    type(type),
    color(1.0f, 1.0f, 1.0f, 10.0f)
{
    glGenBuffers(1, &lightBlockBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
    GLsizeiptr bufferSize = sizeof(color) + sizeof(parent->getPosition());
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, 0, GL_DYNAMIC_DRAW); // Initialize as empty
}

Light::~Light()
{
    glDeleteBuffers(1, &lightBlockBuffer);
}

void Light::execute()
{
    glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
    GLintptr offset = 0;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(color), glm::value_ptr(color));
    offset += sizeof(color);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(parent->getPosition()), glm::value_ptr(parent->getPosition()));
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING_POINT, lightBlockBuffer);
}

void Light::setColor(const glm::vec4& color)
{
    this->color = color;
}

void Light::setDirection(const glm::vec3& direction)
{
    this->direction = direction;
}

Light::Type Light::getLightType() const
{
    return type;
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

