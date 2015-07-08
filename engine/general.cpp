#include "general.h"

namespace moar
{

float Plane::distance(const glm::vec3& p) const
{
    return (d + glm::dot(normal, p));
}

Plane calculatePlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    glm::vec3 a = p1 - p2;
    glm::vec3 b = p3 - p2;
    Plane plane;
    plane.point = p2;
    plane.normal = glm::normalize(glm::cross(b, a));
    plane.d = -(glm::dot(plane.point, plane.normal));
    return plane;
}

}
