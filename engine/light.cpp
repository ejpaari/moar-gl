#include "light.h"
#include "object.h"

namespace moar
{

GLint Light::shader = 0;

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
    glGetIntegerv(GL_CURRENT_PROGRAM, &shader);
    glUniform3f(glGetUniformLocation(shader, "lightPos"), parent->getPosition().x, parent->getPosition().y, parent->getPosition().z);
    glUniform3f(glGetUniformLocation(shader, "lightColor"), color.x, color.y, color.z);
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

