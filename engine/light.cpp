#include "light.h"
#include "object.h"

namespace moar
{

Light::Light() :
    color(1.0f, 1.0f, 1.0f),
    power(10.0f)
{
}

Light::~Light()
{
}

void Light::execute()
{
    glUniform3f(glGetUniformLocation(currentShader, "lightPos"), parent->getPosition().x, parent->getPosition().y, parent->getPosition().z);
    glUniform3f(glGetUniformLocation(currentShader, "lightColor"), color.x, color.y, color.z);
    glUniform1f(glGetUniformLocation(currentShader, "lightPower"), power);
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

