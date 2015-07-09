#include "plane.h"

#include <algorithm>
#include <utility>

namespace moar
{

Plane::Plane() :
    d(0.0f)
{

}

Plane::Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    glm::vec3 a = p1 - p2;
    glm::vec3 b = p3 - p2;
    point = p2;
    normal = glm::normalize(glm::cross(a, b));
    d = -(glm::dot(point, normal));
}

Plane::~Plane()
{
}

Plane::Plane(const Plane& rhs) :
   point(rhs.point),
   normal(rhs.normal),
   d(rhs.d)
{
}

Plane::Plane(Plane&& rhs) :
   point(std::move(rhs.point)),
   normal(std::move(rhs.normal)),
   d(rhs.d)
{
}

Plane& Plane::operator=(Plane rhs) {
   std::swap(point, rhs.point);
   std::swap(normal, rhs.normal);
   std::swap(d, rhs.d);
   return *this;
}

Plane& Plane::operator=(Plane&& rhs) {
   point = std::move(rhs.point);
   normal = std::move(rhs.normal);
   d = std::move(rhs.d);
   return *this;
}

float Plane::distance(const glm::vec3& p) const
{
    return (d + glm::dot(normal, p));
}

}
