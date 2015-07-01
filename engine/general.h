#ifndef GENERAL
#define GENERAL

namespace moar
{
struct Quad
{
    glm::vec3 topLeft;
    glm::vec3 topRight;
    glm::vec3 bottomLeft;
    glm::vec3 bottomRight;
};

struct Plane
{
    glm::vec3 point;
    glm::vec3 normal;
};
}

#endif // GENERAL

