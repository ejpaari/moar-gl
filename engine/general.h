#ifndef GENERAL
#define GENERAL

#include <glm/glm.hpp>

namespace moar
{
struct Quad
{
    glm::vec3 topLeft;
    glm::vec3 topRight;
    glm::vec3 bottomLeft;
    glm::vec3 bottomRight;
};

// ToDo: Create own class.
struct Plane
{
    glm::vec3 point;
    glm::vec3 normal;
    float d;

    float distance(const glm::vec3& p) const;
};

Plane calculatePlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
}

#endif // GENERAL

