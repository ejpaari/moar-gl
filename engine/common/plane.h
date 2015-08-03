#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>

namespace moar
{

class Plane
{
public:
    Plane();
    Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
    ~Plane();
    Plane(const Plane& rhs);
    Plane(Plane&& rhs);
    Plane& operator=(const Plane& rhs);
    Plane& operator=(Plane&& rhs);

    float distance(const glm::vec3& p) const;

private:
    glm::vec3 point;
    glm::vec3 normal;
    float d;
};

}

#endif // PLANE_H
