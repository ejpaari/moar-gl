#ifndef OBJECT_H
#define OBJECT_H

#include "component.h"
#include "shader.h"
#include "renderer.h"
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

    // Todo: Improve interface, user should not have access to render etc.
    void prepareLight();
    void render(const Shader* shader);

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
    glm::mat4x4 getModelMatrix() const;
    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getLeft() const;
    std::string getName() const;

    bool hasComponent(const std::string& name) const;

    template<typename T>
    T* addComponent();

    template<typename T>
    T* getComponent() const;

protected:
    static unsigned int idCounter;

    unsigned int id;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 left;    

    Component* material;
    Component* renderer;
    Component* light;
    std::vector<std::shared_ptr<Component>> allComponents;    

    GLuint transformationBlockBuffer;

    std::string name;
};

template<typename T>
T* Object::addComponent()
{
    T* component = getComponent<T>();
    if (component != nullptr) {
        return component;
    }

    std::shared_ptr<Component> comp;
    if (typeid(T) == typeid(Renderer)) {
        comp.reset(new Renderer);
        renderer = comp.get();
    }
    if (typeid(T) == typeid(Light)) {
        comp.reset(new Light);
        light = comp.get();
    }
    if (typeid(T) == typeid(Material)) {
        comp.reset(new Material);
        material = comp.get();
    }

    allComponents.push_back(comp);
    component = dynamic_cast<T*>(comp.get());
    if (component != nullptr) {
        component->setParent(this);
        return component;
    } else {
        std::cerr << "ERROR: Component casting failed when adding a component" << std::endl;
        return nullptr;
    }
}

template<typename T>
T* Object::getComponent() const
{
    for (unsigned int i = 0; i < allComponents.size(); ++i) {
        if (typeid(*allComponents[i].get()) == typeid(T)) {
            return dynamic_cast<T*>(allComponents[i].get());
        }
    }
    return nullptr;
}

} // moar

#endif // OBJECT_H
