#include "camera.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
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
    position = pos;
    *viewMatrix = glm::mat4(glm::lookAt(position, forward, up));
}

void Camera::move(const glm::vec3& translation)
{
    position += translation;
    *viewMatrix = glm::mat4(glm::lookAt(position, forward, up));
}

} // moar
