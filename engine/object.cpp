#include "object.h"

#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

namespace moar
{

const glm::vec3 Object::FORWARD = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 Object::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Object::LEFT = glm::vec3(1.0f, 0.0f, 0.0f);

Object::Object() :
    position(0.0f, 0.0f, 0.0f),
    rotation(0.0f, 0.0f, 0.0f),
    scale(1.0f, 1.0f, 1.0f),
    forward(FORWARD),
    up(UP),
    left(LEFT),
    material(nullptr),
    renderer(nullptr)
{
}

Object::~Object()
{
}

void Object::execute()
{
    if (material != nullptr) {
        material->execute();
    }
    if (renderer != nullptr) {
        renderer->execute();
    }

    for (unsigned int i = 0; i < components.size(); ++i) {
        components[i]->execute();
    }
}

glm::mat4x4 Object::getModelMatrix() const
{
    glm::mat4x4 modelMatrix =
            glm::translate(position) *
            glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
            glm::scale(scale);
    return modelMatrix;
}

glm::vec3 Object::getForward() const
{
    glm::vec4 v =
            glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
            glm::vec4(FORWARD.x, FORWARD.y, FORWARD.z, 0.0f);
    return glm::vec3(v.x, v.y, v.z);
}

glm::vec3 Object::getUp() const
{
    glm::vec4 v =
            glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
            glm::vec4(UP.x, UP.y, UP.z, 0.0f);
    return glm::vec3(v.x, v.y, v.z);
}

glm::vec3 Object::getLeft() const
{
    glm::vec4 v =
            glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
            glm::vec4(LEFT.x, LEFT.y, LEFT.z, 0.0f);
    return glm::vec3(v.x, v.y, v.z);
}

bool Object::setComponent(Component* comp)
{
    // Todo: type enum + switch
    comp->setParent(this);
    if (comp->getType() == "Material") {
        material.reset(comp);
        return true;
    } else if (comp->getType() == "Renderer") {
        renderer.reset(comp);
        return true;
    } else {
        for (unsigned int i = 0; i < components.size(); ++i) {
            if (components[i]->getType() == comp->getType()) {
                components[i].reset(comp);
                return true;
            }
        }
    }
    return false;
}

Component* Object::getComponent(const std::string& name) const
{
    // Todo: use type enum and switch case
    if (name == "Material") {
        return material.get();
    } else if (name == "Renderer") {
        return renderer.get();
    }
    for (unsigned int i = 0; i < components.size(); ++i) {
        if (components[i]->getType() == name) {
            return components[i].get();
        }
    }
    return nullptr;
}

} // moar
