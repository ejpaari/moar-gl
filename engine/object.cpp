#include "object.h"
#include "constants.h"

#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <assert.h>

namespace moar
{

const glm::vec3 Object::FORWARD = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 Object::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Object::LEFT = glm::vec3(1.0f, 0.0f, 0.0f);

const glm::mat4* Object::projection = nullptr;
const glm::mat4* Object::view = nullptr;

GLint Object::currentShader = 0;

Object::Object() :
    position(0.0f, 0.0f, 0.0f),
    rotation(0.0f, 0.0f, 0.0f),
    scale(1.0f, 1.0f, 1.0f),
    forward(FORWARD),
    up(UP),
    left(LEFT),
    material(nullptr),
    renderer(nullptr),
    light(nullptr)
{
}

Object::~Object()
{
}

void Object::executeCustomComponents()
{
    for (unsigned int i = 0; i < customComponents.size(); ++i) {
        customComponents[i]->execute();
    }
}

void Object::prepareRender()
{
    assert(material != nullptr);
    material->execute();
    assert(renderer != nullptr);
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
    glm::mat4x4 model = getModelMatrix();
    // Todo: uniform block.
    glUniformMatrix4fv(M_LOCATION, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(V_LOCATION, 1, GL_FALSE, glm::value_ptr(*view));
    glUniformMatrix4fv(MV_LOCATION, 1, GL_FALSE, glm::value_ptr((*view) * model));
    glUniformMatrix4fv(P_LOCATION, 1, GL_FALSE, glm::value_ptr(*projection));
    glUniformMatrix4fv(MVP_LOCATION, 1, GL_FALSE, glm::value_ptr((*projection) * (*view) * model));
}

void Object::prepareLight()
{
    assert(light != nullptr);
    light->execute();
}

void Object::render()
{
    assert(renderer != nullptr);
    renderer->execute();
}

void Object::move(const glm::vec3& translation)
{
    position += translation;
}

void Object::rotate(const glm::vec3& axis, float amount)
{
    rotation += axis * amount;
}

void Object::setPosition(const glm::vec3& position)
{
    this->position = position;
}

void Object::setRotation(const glm::vec3& rotation)
{
    this->rotation = rotation;
}

void Object::setScale(const glm::vec3& scale)
{
    this->scale = scale;
}

glm::vec3 Object::getPosition()
{
    return position;
}

glm::vec3 Object::getRotation()
{
    return rotation;
}

glm::vec3 Object::getScale()
{
    return scale;
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

void Object::addComponent(Component *comp)
{
    comp->setParent(this);
    std::unique_ptr<Component> component(comp);
    bool componentExists = false;
    for (unsigned int i = 0; i < allComponents.size(); ++i) {
        if (allComponents[i]->getName() == comp->getName()) {
            allComponents[i].reset(comp);
            componentExists = true;
        }
    }

    if (!componentExists) {
        allComponents.push_back(std::move(component));
    }

    switch (comp->getType()) {
    case Component::RENDERER:
        renderer = comp;
        break;
    case Component::MATERIAL:
        material = comp;
        break;
    case Component::LIGHT:
        light = comp;
        break;
    case Component::CUSTOM:
        for (auto custom : customComponents) {
            if (custom->getName() == comp->getName()) {
                custom = comp;
                return;
            }
        }
        break;
    default:
        return;
    }
}

bool Object::hasComponent(const std::string& name) const
{
    for (unsigned int i = 0; i < allComponents.size(); ++i) {
        if (allComponents[i]->getName() == name) {
            return true;
        }
    }
    return false;
}

} // moar
