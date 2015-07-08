#include "camera.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <boost/math/constants/constants.hpp>
#include <cmath>

namespace moar
{

const float Camera::ROTATION_LIMIT = 85.0f * boost::math::constants::degree<double>();

Camera::Camera() :
    FOV(45.0f),
    ratio(4.0f / 3.0f),
    nearClipDistance(0.1f),
    farClipDistance(100.0f),
    viewMatrix(new glm::mat4(glm::lookAt(position, forward, up))),
    projectionMatrix(new glm::mat4(glm::perspective(FOV, ratio, nearClipDistance, farClipDistance)))
{
    calculateFrustum();
}

Camera::~Camera()
{
}

void Camera::setPosition(const glm::vec3& pos)
{
    Object::setPosition(pos);
    calculateViewMatrix();
}

void Camera::move(const glm::vec3& translation)
{
    Object::move(translation);
    calculateViewMatrix();
}

void Camera::rotate(const glm::vec3& axis, float amount) {
    Object::rotate(axis, amount);
    if (rotation.x > ROTATION_LIMIT) {
        rotation.x = ROTATION_LIMIT;
    }
    if (rotation.x < -ROTATION_LIMIT) {
        rotation.x = -ROTATION_LIMIT;
    }
    calculateViewMatrix();
}

const glm::mat4* Camera::getViewMatrixPointer() const
{
    return viewMatrix.get();
}

const glm::mat4* Camera::getProjectionMatrixPointer() const
{
    return projectionMatrix.get();
}

bool Camera::sphereInsideFrustum(const glm::vec3& point, float radius) const
{
    for (int i = 0; i < SIZE; ++i) {
        float distance = frustumPlanes[i].distance(point);
        if (distance < -radius) {
            return false;
        }
    }
    return true;
}

void Camera::calculateViewMatrix()
{
    *viewMatrix = glm::mat4(glm::lookAt(position, position + getForward(), up));
}

void Camera::calculateFrustum()
{
    glm::vec2 nearClipPlaneSize = getClipPlaneSize(nearClipDistance);
    glm::vec2 farClipPlaneSize = getClipPlaneSize(farClipDistance);

    // ToDo: Clip quads are not really necessary.
    nearClipQuad = getClipPlaneQuad(nearClipDistance, nearClipPlaneSize);
    farClipQuad = getClipPlaneQuad(farClipDistance, farClipPlaneSize);
    frustumPlanes[TOP] = calculatePlane(farClipQuad.topLeft, nearClipQuad.topLeft, nearClipQuad.topRight);
    frustumPlanes[LEFT] = calculatePlane(farClipQuad.bottomLeft, nearClipQuad.bottomLeft, nearClipQuad.topLeft);
    frustumPlanes[RIGHT] = calculatePlane(farClipQuad.topRight, nearClipQuad.topRight, nearClipQuad.bottomRight);
    frustumPlanes[BOTTOM] = calculatePlane(farClipQuad.bottomRight, nearClipQuad.bottomRight, nearClipQuad.bottomLeft);
    frustumPlanes[FRONT] = calculatePlane(nearClipQuad.topRight, nearClipQuad.topLeft, nearClipQuad.bottomLeft);
    frustumPlanes[BACK] = calculatePlane(farClipQuad.topLeft, farClipQuad.topRight, farClipQuad.bottomRight);
}

glm::vec2 Camera::getClipPlaneSize(float distance)
{
    glm::vec2 size;
    size.x = 2.0f * tan(FOV / 2.0f) * distance;
    size.y = size.x * ratio;
    return size;
}

Quad Camera::getClipPlaneQuad(float distance, glm::vec2 size)
{
    glm::vec3 center = Object::FORWARD * distance;
    Quad quad;
    quad.topLeft = center + (Object::UP * size.y / 2.0f) + (Object::LEFT * size.x / 2.0f);
    quad.topRight = center + (Object::UP * size.y / 2.0f) - (Object::LEFT * size.x / 2.0f);
    quad.bottomLeft = center - (Object::UP * size.y / 2.0f) + (Object::LEFT * size.x / 2.0f);
    quad.bottomRight = center - (Object::UP * size.y / 2.0f) - (Object::LEFT * size.x / 2.0f);
    return quad;
}

} // moar














