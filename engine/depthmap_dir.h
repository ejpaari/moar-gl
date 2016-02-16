#ifndef DEPTHMAP_DIR_H
#define DEPTHMAP_DIR_H

#include "depthmap.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace moar
{

class DepthMapDirectional : public DepthMap
{
public:
    explicit DepthMapDirectional();
    virtual ~DepthMapDirectional();
    DepthMapDirectional(const DepthMapDirectional&) = delete;
    DepthMapDirectional(DepthMapDirectional&&) = delete;
    DepthMapDirectional& operator=(const DepthMapDirectional&) = delete;
    DepthMapDirectional& operator=(DepthMapDirectional&&) = delete;

    virtual bool init();
    virtual void bind(const glm::vec3& lightPos, const glm::vec3& lightDir);
    virtual void activate();

private:
    GLuint depthTexture;
    glm::vec4 frustum = glm::vec4(-6.0f, 6.0f, -6.0f, 6.0f);
    glm::mat4 projectionMatrix = glm::ortho(frustum.x, frustum.y, frustum.z, frustum.w, nearClipDistance, farClipDistance);
    glm::mat4 lightSpaceMatrix;
};

} // moar

#endif // DEPTHMAP_DIR_H
