#include "object.h"

#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace moar
{

Object::Object() :
    scale(1.0f, 1.0f, 1.0f),
    forward(0.0f, 0.0f, 1.0f),
    up(0.0f, 1.0f, 0.0f),
    left(1.0, 0.0f, 0.0f)
{
}

Object::~Object()
{
}

glm::mat4x4 Object::getModelMatrix() const
{
    glm::mat4x4 modelMatrix;
    modelMatrix =
            glm::translate(position) *
            glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
            glm::scale(scale) *
            modelMatrix;
    return modelMatrix;
}

} // moar
