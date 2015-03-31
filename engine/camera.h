#ifndef CAMERA_H
#define CAMERA_H

#include "object.h"

#include <glm/glm.hpp>
#include <memory>

namespace moar
{

class Camera : public Object
{
public:
    explicit Camera();
    virtual ~Camera();
    Camera(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera& operator=(Camera&&) = delete;

    virtual void setPosition(const glm::vec3& pos);
    virtual void move(const glm::vec3& translation);


    const glm::mat4* getViewMatrixPointer() const { return viewMatrix.get(); }
    const glm::mat4* getProjectionMatrixPointer() const { return projectionMatrix.get(); }

private:
    float FOV;
    float ratio;
    float nearClipPlane;
    float farClipPlane;

    std::unique_ptr<glm::mat4> viewMatrix;
    std::unique_ptr<glm::mat4> projectionMatrix;
};

} // moar

#endif // CAMERA_H
