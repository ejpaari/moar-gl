#include "light.h"
#include "object.h"
#include "common/globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

namespace moar
{

GLuint Light::lightBlockBuffer = 0; // Initialized by friend class renderer
constexpr float DIST = 10.0f / static_cast<float>(sqrt(3.0)); // Heuristic

Light::Light()
{
}

Light::~Light()
{    
}

void Light::setType(Light::Type type)
{
    this->type = type;
    G_COMPONENT_CHANGED = true;
}

void Light::setShadowingEnabled(bool enabled)
{
    shadowingEnabled = enabled;
}

void Light::setColor(const glm::vec4& color)
{
    this->color = color;
    calculateRange();
}

Light::Type Light::getLightType() const
{
    return type;
}

bool Light::isShadowingEnabled() const
{
    return shadowingEnabled;
}

float Light::getRange() const
{
    return range;
}

void Light::setUniforms(const glm::vec3& position, const glm::vec3& forward)
{
    glBindBuffer(GL_UNIFORM_BUFFER, lightBlockBuffer);
    GLintptr offset = 0;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, 16, glm::value_ptr(color));
    offset += 16;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, glm::value_ptr(position));
    offset += 16;
    glBufferSubData(GL_UNIFORM_BUFFER, offset, 12, glm::value_ptr(forward));
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING_POINT, lightBlockBuffer);
}

void Light::calculateRange()
{
    range = DIST * sqrt(color.w);
}

} // moar

