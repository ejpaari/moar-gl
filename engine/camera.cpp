#include "camera.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <cassert>

namespace moar
{

const float Camera::ROTATION_LIMIT = 85.0f * boost::math::constants::degree<float>();

Camera::Camera(float fov, float ratio, float nearClip, float farClip) :
    FOV(fov),
    ratio(ratio),
    nearClipDistance(nearClip),
    farClipDistance(farClip),
    viewMatrix(new glm::mat4(glm::lookAt(position, forward, up))),
    projectionMatrix(new glm::mat4(glm::perspective(FOV, ratio, nearClipDistance, farClipDistance)))
{
    calculateFrustum();
}

Camera::~Camera()
{
}

void Camera::rotate(const glm::vec3& axis, float amount) {
    Object::rotate(axis, amount);
    if (rotation.x > ROTATION_LIMIT) {
        rotation.x = ROTATION_LIMIT;
    }
    if (rotation.x < -ROTATION_LIMIT) {
        rotation.x = -ROTATION_LIMIT;
    }
}

void Camera::setRotation(const glm::vec3& rotation) {
    Object::setRotation(rotation);
    if (this->rotation.x > ROTATION_LIMIT) {
        this->rotation.x = ROTATION_LIMIT;
    }
    if (this->rotation.x < -ROTATION_LIMIT) {
        this->rotation.x = -ROTATION_LIMIT;
    }
}

const glm::mat4* Camera::getViewMatrixPointer() const
{
    return viewMatrix.get();
}

const glm::mat4* Camera::getProjectionMatrixPointer() const
{
    return projectionMatrix.get();
}

float Camera::getFarClipDistance() const
{
    return farClipDistance;
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

Postprocess* Camera::addPostprocess(const std::string& name, GLuint shader, int priority)
{
    for (const auto& p : postprocs) {
        if (p.getName() == name) {
            std::cerr << "WARNING: Post process effect already exists: " << name << "\n";
            return nullptr;
        }
    }

    Postprocess proc(name, shader, priority);

    if (postprocs.empty() || priority >= postprocs.back().getPriority()) {
        postprocs.push_back(proc);
        return &postprocs.back();
    } else {
        bool postprocAdded = false;
        auto iter = postprocs.begin();
        for (; iter != postprocs.end(); ++iter) {
            if (priority <= iter->getPriority()) {
                iter = postprocs.insert(iter, proc);
                postprocAdded = true;
                break;
            }
        }
        if (postprocAdded) {
            return &*iter;
        } else {
            std::cerr << "WARNING: Could not add post processing effect: " << name << "\n";
            return nullptr;
        }
    }
}

bool Camera::removePostprocess(const std::string& name)
{
    for (auto iter = postprocs.begin(); iter != postprocs.end(); ++iter) {
        if (iter->getName() == name) {
            postprocs.erase(iter);
            return true;
        }
    }
    return false;
}

const std::list<Postprocess>& Camera::getPostprocesses() const
{
    return postprocs;
}

unsigned int Camera::getBloomIterations() const
{
    return bloomIterations;
}

void Camera::setBloomIterations(unsigned int iterations)
{
    assert(iterations % 2 == 0);
    bloomIterations = iterations;
}

bool Camera::isHDREnabled() const
{
    return useHDR;
}

void Camera::setHDREnabled(bool status)
{
    useHDR = status;
}

void Camera::updateViewMatrix()
{
    *viewMatrix = glm::mat4(glm::lookAt(position, position + getForward(), up));
}

void Camera::calculateFrustum()
{
    glm::vec2 nearClipPlaneSize = getClipPlaneSize(nearClipDistance);
    glm::vec2 farClipPlaneSize = getClipPlaneSize(farClipDistance);

    Quad nearClipQuad = getClipPlaneQuad(nearClipDistance, nearClipPlaneSize);
    Quad farClipQuad = getClipPlaneQuad(farClipDistance, farClipPlaneSize);
    frustumPlanes[TOP] = Plane(farClipQuad.topLeft, nearClipQuad.topLeft, nearClipQuad.topRight);
    frustumPlanes[LEFT] = Plane(farClipQuad.bottomLeft, nearClipQuad.bottomLeft, nearClipQuad.topLeft);
    frustumPlanes[RIGHT] = Plane(farClipQuad.topRight, nearClipQuad.topRight, nearClipQuad.bottomRight);
    frustumPlanes[BOTTOM] = Plane(farClipQuad.bottomRight, nearClipQuad.bottomRight, nearClipQuad.bottomLeft);
    frustumPlanes[FRONT] = Plane(nearClipQuad.topRight, nearClipQuad.topLeft, nearClipQuad.bottomLeft);
    frustumPlanes[BACK] = Plane(farClipQuad.topLeft, farClipQuad.topRight, farClipQuad.bottomRight);
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
    glm::vec3 y = Object::UP * size.y / 2.0f;
    glm::vec3 x = Object::LEFT * size.x / 2.0f;
    Quad quad;
    quad.topLeft = center + y + x;
    quad.topRight = center + y - x;
    quad.bottomLeft = center - y + x;
    quad.bottomRight = center - y - x;
    return quad;
}

} // moar
