#ifndef OBJECT_H
#define OBJECT_H

#include "component.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace moar
{

class Object
{
public:
    static const glm::vec3 FORWARD;
    static const glm::vec3 UP;
    static const glm::vec3 LEFT;

    explicit Object();
    virtual ~Object();
    Object(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(const Object&) = delete;
    Object& operator=(Object&&) = delete;

    void execute();

    virtual void move(const glm::vec3& translation) { position += translation; }
    virtual void rotate(const glm::vec3& axis, float amount) { rotation += axis * amount; }

    virtual void setPosition(const glm::vec3& position) { this->position = position; }
    virtual void setRotation(const glm::vec3& rotation) { this->rotation = rotation; }
    virtual void setScale(const glm::vec3& scale) { this->scale = scale; }

    // Todo: transformation component
    glm::mat4x4 getModelMatrix() const;
    glm::vec3 getPosition() { return position; }
    glm::vec3 getRotation() { return rotation; }
    glm::vec3 getScale() { return scale; }
    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getLeft() const;

    bool setComponent(Component* comp);

    Component* getComponent(const std::string& name) const;

protected:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 left;

    std::unique_ptr<Component> material;
    std::unique_ptr<Component> renderer;
    std::vector<std::unique_ptr<Component>> components;
};

} // moar

#endif // OBJECT_H