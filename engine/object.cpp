#include "object.h"
#include "common/globals.h"

#define GLM_FORCE_RADIANS
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <assert.h>

namespace moar
{

const glm::vec3 Object::FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 Object::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Object::LEFT = glm::vec3(-1.0f, 0.0f, 0.0f);

const glm::mat4* Object::projection = nullptr;
const glm::mat4* Object::view = nullptr;

unsigned int Object::idCounter = 0;
GLuint Object::transformationBlockBuffer = 0;
bool Object::bufferCreated = false;
Material* Object::defaultMaterial = nullptr;

Object::Object() :
    id(idCounter)
{
    ++idCounter;
    if (!bufferCreated) {
        glGenBuffers(1, &transformationBlockBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, transformationBlockBuffer);
        GLsizeiptr bufferSize = 4 * sizeof(*projection);
        glBufferData(GL_UNIFORM_BUFFER, bufferSize, 0, GL_DYNAMIC_DRAW); // Initialize as empty
        bufferCreated = true;
    }
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

void Object::setShadowReceiver(bool receiver)
{
    shadowReceiver = receiver;
}

bool Object::isShadowCaster() const
{
    return shadowCaster;
}

void Object::setModel(Model* model)
{
    this->model = model;
    meshObjects.clear();
    for (const auto& mesh : model->getMeshes()) {
        // Todo: Mesh default material from the loaded model.
        // Todo: Check that material is not null.
        MeshObject mo = {mesh.get(), defaultMaterial, this};
        meshObjects.push_back(mo);
    }
    COMPONENT_CHANGED = true;
}

std::vector<Object::MeshObject>& Object::getMeshObjects()
{
    return meshObjects;
}

void Object::setMeshDefaultMaterial(Material* material)
{
    defaultMaterial = material;
}

void Object::prepareLight()
{
    if (light) {
        light->execute(position, forward);
    }
}

void Object::setMatrixUniforms(const Shader* shader)
{
    glBindBuffer(GL_UNIFORM_BUFFER, transformationBlockBuffer);
    GLintptr matrixSize = sizeof(modelMatrix);
    glBufferSubData(GL_UNIFORM_BUFFER, 0 * matrixSize, matrixSize, glm::value_ptr(modelMatrix));
    // Todo: It is unnecessary to update view-matrix each time.
    glBufferSubData(GL_UNIFORM_BUFFER, 1 * matrixSize, matrixSize, glm::value_ptr(*view));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * matrixSize, matrixSize, glm::value_ptr((*view) * modelMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, 3 * matrixSize, matrixSize, glm::value_ptr((*projection) * (*view) * modelMatrix));
    glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORMATION_BINDING_POINT, transformationBlockBuffer);

    if (shadowReceiver && shader->hasUniform(RECEIVE_SHADOWS_LOCATION)) {
        glUniform1i(RECEIVE_SHADOWS_LOCATION, static_cast<int>(shadowReceiver));
    }
}

void Object::updateModelMatrix()
{
    modelMatrix =
            glm::translate(position) *
            glm::yawPitchRoll(rotation.y, rotation.x, rotation.z) *
            glm::scale(scale);
}

glm::mat4x4 Object::getModelMatrix() const
{
    return modelMatrix;
}

template<>
Model* Object::getComponent<Model>() const
{
    return model;
}

template<>
Light* Object::getComponent<Light>() const
{
    return light.get();
}

} // moar
