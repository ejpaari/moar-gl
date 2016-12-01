#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"
#include "postprocess.h"
#include "common/quad.h"
#include "common/plane.h"

#include <glm/glm.hpp>

#include <memory>
#include <list>
#include <string>

namespace moar
{

class Camera : public Object
{
    friend class Engine;

public:
    explicit Camera(float fov = 45.0f, float ratio = 4.0f / 3.0f, float nearClip = 0.1f, float farClip = 100.0f);
    virtual ~Camera();
    Camera(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera& operator=(Camera&&) = delete;

    virtual void rotate(const glm::vec3& axis, float amount);
    virtual void setRotation(const glm::vec3& rotation);

    const glm::mat4* getViewMatrixPointer() const;
    const glm::mat4* getProjectionMatrixPointer() const;
    float getFarClipDistance() const;

    bool sphereInsideFrustum(const glm::vec3& point, float radius) const;

    Postprocess* addPostprocess(const std::string& name, GLuint shader, int priority);
    bool removePostprocess(const std::string& name);
    const std::list<Postprocess>& getPostprocesses() const;

    unsigned int getBloomIterations() const;
    void setBloomIterations(unsigned int iterations);
    bool isHDREnabled() const;
    void setHDREnabled(bool status);

private:
    enum Side
    {
        TOP = 0,
        LEFT = 1,
        RIGHT = 2,
        BOTTOM = 3,
        FRONT = 4,
        BACK = 5,
        SIZE = 6
    };

    static const float ROTATION_LIMIT;

    void updateViewMatrix();

    void calculateFrustum();
    glm::vec2 getClipPlaneSize(float distance);
    Quad getClipPlaneQuad(float distance, glm::vec2 size);

    float FOV;
    float ratio;
    float nearClipDistance;
    float farClipDistance;
    Plane frustumPlanes[SIZE];

    std::unique_ptr<glm::mat4> viewMatrix;
    std::unique_ptr<glm::mat4> projectionMatrix;

    std::list<Postprocess> postprocs;
    unsigned int bloomIterations = 0;
    bool useHDR = false;
};

} // moar

#endif // CAMERA_H
