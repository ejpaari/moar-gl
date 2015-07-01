#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"
#include "general.h"

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

    // Todo: frustum culling

private:

    enum Side
    {
        TOP = 1, LEFT = 2, RIGHT = 3, BOTTOM = 4
    };

    void calculateViewMatrix();
    void calculateFrustum();
    glm::vec2 getClipPlaneSize(float distance);
    Quad getClipPlaneQuad(float distance, glm::vec2 size);

    float FOV;
    float ratio;
    float nearClipDistance;
    float farClipDistance;
    Quad farClipPlane;
    Quad nearClipPlane;
    Plane frustumPlanes[4];

    std::unique_ptr<glm::mat4> viewMatrix;
    std::unique_ptr<glm::mat4> projectionMatrix;
};

} // moar

#endif // CAMERA_H
