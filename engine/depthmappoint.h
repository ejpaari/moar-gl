#ifndef DEPTHMAP_POINT_H
#define DEPTHMAP_POINT_H

#include "depthmap.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <array>

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
    virtual void updateUniformValues(const glm::vec3& lightPos, const glm::vec3& lightDir);
    virtual void setUniforms() const;
    virtual void activate() const;
    virtual GLuint getTexture() const;
    virtual GLenum getType() const;

private:
    GLuint depthCubeTexture;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, nearClipDistance, farClipDistance);
    std::array<glm::mat4, 6> lightSpaces;
};

} // moar

#endif // DEPTHMAP_POINT_H
