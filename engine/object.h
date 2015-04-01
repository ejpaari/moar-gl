#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>

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

    virtual void setPosition(const glm::vec3& position) { this->position = position; }
    virtual void setRotation(const glm::vec3& rotation) { this->rotation = rotation; }
    virtual void setScale(const glm::vec3& scale) { this->scale = scale; }

    virtual void move(const glm::vec3& translation) { position += translation; }
    virtual void rotate(const glm::vec3& axis, float amount) { rotation += axis * amount; }

    glm::mat4x4 getModelMatrix() const;
    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getLeft() const;

protected:

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 left;
};

} // moar

#endif // OBJECT_H
