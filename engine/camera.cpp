#include "camera.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <boost/math/constants/constants.hpp>

namespace moar
{

const float Camera::ROTATION_LIMIT = 85.0f * boost::math::constants::degree<double>();

Camera::Camera() :
    FOV(45.0f),
    ratio(4.0f / 3.0f),
    nearClipPlane(0.1f),
    farClipPlane(100.0f),
    viewMatrix(new glm::mat4(glm::lookAt(position, forward, up))),
    projectionMatrix(new glm::mat4(glm::perspective(FOV, ratio, nearClipPlane, farClipPlane)))
{
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
    if (rotation.x > ROTATION_LIMIT) {
        rotation.x = ROTATION_LIMIT;
    }
    if (rotation.x < -ROTATION_LIMIT) {
        rotation.x = -ROTATION_LIMIT;
    }
    updateViewMatrix();
}

void Camera::updateViewMatrix()
{
    *viewMatrix = glm::mat4(glm::lookAt(position, position + getForward(), up));
}

} // moar
