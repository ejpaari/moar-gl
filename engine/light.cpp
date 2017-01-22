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
GLuint Light::lightProjectionBlockBuffer = 0; // Initialized by friend class renderer
constexpr float DIST = 10.0f / 1.7320508f; // Heuristic, sqrt(3)

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

const glm::vec4&Light::getColor() const
{
    return color;
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
    glUniform4f(LIGHT_COLOR_LOCATION, color.r, color.g, color.b, color.a);
    glUniform3f(LIGHT_POS_LOCATION, position.x, position.y, position.z);
    glUniform3f(LIGHT_FORWARD_LOCATION, forward.x, forward.y, forward.z);
}

void Light::calculateRange()
{
    range = DIST * sqrt(color.w);
}

} // moar

