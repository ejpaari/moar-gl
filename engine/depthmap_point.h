#ifndef DEPTHMAP_POINT_H
#define DEPTHMAP_POINT_H

#include "depthmap.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace moar
{

class DepthMapPoint : public DepthMap
{
public:
    explicit DepthMapPoint();
    ~DepthMapPoint();
    DepthMapPoint(const DepthMapPoint&) = delete;
    DepthMapPoint(DepthMapPoint&&) = delete;
    DepthMapPoint& operator=(const DepthMapPoint&) = delete;
    DepthMapPoint& operator=(DepthMapPoint&&) = delete;

    virtual bool init();
    virtual void bind(const glm::vec3& lightPos, const glm::vec3& lightDir = glm::vec3(0.0f, 0.0f, 0.0f));
    virtual void activate();

private:
    GLuint depthCubeTexture;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, nearClipDistance, farClipDistance);
    glm::mat4 lightSpaces[6];
};

} // moar

#endif // DEPTHMAP_POINT_H