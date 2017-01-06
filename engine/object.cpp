#include "object.h"
#include "common/globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace moar
{

const glm::vec3 Object::FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 Object::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Object::LEFT = glm::vec3(-1.0f, 0.0f, 0.0f);

const glm::mat4* Object::projection = nullptr;
const glm::mat4* Object::view = nullptr;

glm::mat4 Object::viewProjection;
unsigned int Object::idCounter = 0;
GLuint Object::transformationBlockBuffer = 0; // Initialized by friend class renderer
Material* Object::defaultMaterial = nullptr;

void Object::updateViewProjectionMatrix()
{
    viewProjection = (*projection) * (*view);
}

void Object::setMeshDefaultMaterial(Material* material)
{
    defaultMaterial = material;
}

void Object::setViewMatrixUniform()
{
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBlockBuffer);
    GLintptr matrixSize = sizeof(modelMatrix);
    glBufferSubData(GL_UNIFORM_BUFFER, 1 * matrixSize, matrixSize, glm::value_ptr(*view));
}

Object::Object() :
    id(++idCounter)
{
}

Object::Object(const std::string& name) :
    id(++idCounter),
    name(name)
{
}

Object::~Object()
{
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

unsigned int Object::getId() const
{
    return id;
}

std::string Object::getName() const
{
    return name;
}

glm::vec3 Object::getPosition() const
{
    return position;
}

glm::vec3 Object::getRotation() const
{
    return rotation;
}

glm::vec3 Object::getScale() const
{
    return scale;
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

void Object::setShadowCaster(bool caster)
{
    shadowCaster = caster;
}

bool Object::isShadowCaster() const
{
    return shadowCaster;
}

std::vector<Object::MeshObject>& Object::getMeshObjects()
{
    return meshObjects;
}

void Object::setUniforms()
{
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBlockBuffer);
    GLintptr matrixSize = sizeof(modelMatrix);
    glBufferSubData(GL_UNIFORM_BUFFER, 0 * matrixSize, matrixSize, glm::value_ptr(modelMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * matrixSize, matrixSize, glm::value_ptr(modelViewMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, 3 * matrixSize, matrixSize, glm::value_ptr(modelViewProjectionMatrix));
    glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORMATION_BINDING_POINT, transformationBlockBuffer);
}

void Object::updateModelMatrix()
{
    modelMatrix =
            glm::translate(position) *
            glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
            glm::scale(scale);
    modelViewMatrix = (*view) * modelMatrix;
    modelViewProjectionMatrix = viewProjection * modelMatrix;
}

glm::mat4x4 Object::getModelMatrix() const
{
    return modelMatrix;
}

} // moar
