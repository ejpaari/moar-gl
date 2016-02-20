#ifndef DEPTHMAP_H
#define DEPTHMAP_H

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace moar
{

class DepthMap
{
public:
    explicit DepthMap();
    virtual ~DepthMap();
    DepthMap(const DepthMap&) = delete;
    DepthMap(DepthMap&&) = delete;
    DepthMap& operator=(const DepthMap&) = delete;
    DepthMap& operator=(DepthMap&&) = delete;

    virtual bool init() = 0;
    virtual void bind(const glm::vec3& lightPos, const glm::vec3& lightDir) = 0;
    virtual void activate() = 0;

    void setSize(int width, int height);

protected:
    bool createFramebuffer(GLuint& framebuffer, GLuint& texture, bool cube);

    int width = 0;
    int height = 0;
    float nearClipDistance = 0.1f;
    float farClipDistance = 100.0f;

    GLuint shader = 0;
    GLuint framebuffer = 0;
};

} // moar

#endif // DEPTHMAP_H
