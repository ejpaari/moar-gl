#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>

namespace moar {

class Object
{
public:
    Object();
    ~Object();
    Object(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(const Object&) = delete;
    Object& operator=(Object&&) = delete;

protected:
    glm::vec3 position;
};

} // moar

#endif // OBJECT_H
