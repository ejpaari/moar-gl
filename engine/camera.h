#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"
#include "common/math.h"
#include "common/plane.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace moar
{

class Camera : public Object
{
public:
    static const float ROTATION_LIMIT;

    // Todo: this should be inherited from component
    explicit Camera();
    virtual ~Camera();
    Camera(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera& operator=(Camera&&) = delete;

    virtual void setPosition(const glm::vec3& pos);

    virtual void move(const glm::vec3& translation);
    virtual void rotate(const glm::vec3& axis, float amount);

    const glm::mat4* getViewMatrixPointer() const;
    const glm::mat4* getProjectionMatrixPointer() const;

    // Todo: frustum culling for box.
    bool sphereInsideFrustum(const glm::vec3& point, float radius) const;

private:

    enum Side
    {
        TOP = 0, LEFT = 1, RIGHT = 2, BOTTOM = 3, FRONT = 4, BACK = 5, SIZE = 6
    };

    void calculateViewMatrix();
    void calculateFrustum();
    glm::vec2 getClipPlaneSize(float distance);
    Quad getClipPlaneQuad(float distance, glm::vec2 size);

    float FOV;
    float ratio;
    float nearClipDistance;
    float farClipDistance;
    Quad farClipQuad;
    Quad nearClipQuad;
    Plane frustumPlanes[SIZE];

    std::unique_ptr<glm::mat4> viewMatrix;
    std::unique_ptr<glm::mat4> projectionMatrix;
};

} // moar

#endif // CAMERA_H
