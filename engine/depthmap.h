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

    virtual bool init(GLuint shader) = 0;
    virtual void bind(const glm::vec3& lightPos, const glm::vec3& lightDir) = 0;
    virtual void activate() = 0;

    void setWidth(int width);
    void setHeight(int height);

protected:
    bool createFramebuffer(GLuint& framebuffer, GLuint& texture, bool cube);

    int width;
    int height;
    float nearClipDistance;
    float farClipDistance;

    GLuint shader;
    GLuint framebuffer;
};

} // moar

#endif // DEPTHMAP_H
