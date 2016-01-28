#ifndef OBJECT_H
#define OBJECT_H

#include "shader.h"
#include "model.h"
#include "material.h"
#include "light.h"

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>
#include <typeinfo>
#include <string>
#include <iostream>

namespace moar
{

class Object
{
    friend class Engine;

public:
    static const glm::vec3 FORWARD;
    static const glm::vec3 UP;
    static const glm::vec3 LEFT;

    static const glm::mat4* projection;
    static const glm::mat4* view;

    explicit Object();
    virtual ~Object();
    Object(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(const Object&) = delete;
    Object& operator=(Object&&) = delete;

    virtual void move(const glm::vec3& translation);
    virtual void rotate(const glm::vec3& axis, float amount);

    virtual void setPosition(const glm::vec3& position);
    virtual void setRotation(const glm::vec3& rotation);
    virtual void setScale(const glm::vec3& scale);
    void setName(const std::string& name);

    unsigned int getId() const;
    glm::vec3 getPosition() const;
    glm::vec3 getRotation() const;
    glm::vec3 getScale() const;    
    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getLeft() const;
    std::string getName() const;

    void setShadowCaster(bool caster);
    void setShadowReceiver(bool receiver);
    bool isShadowCaster() const;

    // Todo: Is this kind of an interface good? Think about component / model.
    void setModel(const Model* model);
    const Model* getModel() const;

    // Todo: Temporary, should be moved to Mesh-class.
    void setMaterial(Material* material);
    Material* getMaterial() const;

    template<typename T>
    T* addComponent();

    template<typename T>
    T* getComponent() const;

    template<typename T>
    bool hasComponent() const;

protected:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 forward = FORWARD;
    glm::vec3 up = UP;
    glm::vec3 left = LEFT;

private:
    void prepareLight();
    void render(const Shader* shader);
    void updateModelMatrix();
    glm::mat4x4 getModelMatrix() const;

    static unsigned int idCounter;
    static GLuint transformationBlockBuffer;
    static bool bufferCreated;

    unsigned int id;
    std::string name;

    bool shadowCaster = true;
    bool shadowReceiver = true;

    glm::mat4x4 modelMatrix;

    std::unique_ptr<Light> light = nullptr;
    const Model* model = nullptr;
    Material* material = nullptr;
};

// Todo: Maybe template specialization would be better? Or remove these completely?

template<typename T>
T* Object::addComponent()
{
    COMPONENT_CHANGED = true;
    if (typeid(T) == typeid(Light)) {
        light.reset(new Light);
        return reinterpret_cast<T*>(light.get());
    }

    std::cerr << "ERROR: Could not add a component\n";
    return nullptr;
}

template<typename T>
T* Object::getComponent() const
{
    if (typeid(T) == typeid(Light)) {
        return reinterpret_cast<T*>(light.get());
    }

    return nullptr;
}

template<typename T>
bool Object::hasComponent() const
{
    return getComponent<T>() != nullptr;
}

} // moar

#endif // OBJECT_H
