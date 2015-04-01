#include "camera.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace moar
{

Camera::Camera() :
    FOV(45.0f),
    ratio(4.0f / 3.0f),
    nearClipPlane(0.1f),
    farClipPlane(100.0f),
    viewMatrix(new glm::mat4(glm::lookAt(position, forward, up))),
    projectionMatrix(new glm::mat4(glm::perspective(FOV, ratio, nearClipPlane, farClipPlane)))
{
    position = glm::vec3(0.0f, 0.0f, -5.0f);
}

Camera::~Camera()
{

}

void Camera::setPosition(const glm::vec3& pos)
{
    Object::setPosition(pos);
    updateViewMatrix();
}

void Camera::move(const glm::vec3& translation)
{
    Object::move(translation);
    updateViewMatrix();
}

void Camera::rotate(const glm::vec3& axis, float amount) {
    Object::rotate(axis, amount);
    updateViewMatrix();
}

void Camera::updateViewMatrix()
{
    *viewMatrix = glm::mat4(glm::lookAt(position, position + getForward(), up));
}

} // moar
