#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>

namespace moar
{

class Object
{
public:
    explicit Object();
    virtual ~Object();
    Object(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(const Object&) = delete;
    Object& operator=(Object&&) = delete;

    virtual void setPosition(const glm::vec3& pos) { position = pos; }
    virtual void move(const glm::vec3& translation) { position += translation; }

protected:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 forward;
    glm::vec3 up;
};

} // moar

#endif // OBJECT_H
