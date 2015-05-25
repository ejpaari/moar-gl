#ifndef OBJECT_H
#define OBJECT_H

#include "component.h"

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>
#include <typeinfo>

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

    void executeCustomComponents();
    void prepareRender();
    void prepareLight();
    void render();

    virtual void move(const glm::vec3& translation);
    virtual void rotate(const glm::vec3& axis, float amount);

    virtual void setPosition(const glm::vec3& position);
    virtual void setRotation(const glm::vec3& rotation);
    virtual void setScale(const glm::vec3& scale);

    glm::vec3 getPosition();
    glm::vec3 getRotation();
    glm::vec3 getScale();
    glm::mat4x4 getModelMatrix() const;    
    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getLeft() const;

    void addComponent(Component* comp);
    bool hasComponent(const std::string& name) const;

    template<typename T>
    T* getComponent();

protected:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 left;    

    Component* material;
    Component* renderer;
    Component* light;
    std::vector<Component*> customComponents;
    std::vector<std::unique_ptr<Component>> allComponents;

    static GLint currentShader;

    GLuint transformationBlockBuffer;
};

template<typename T>
T* Object::getComponent()
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
