#ifndef SHADOWMAP_H
#define SHADOWMAPH

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace moar
{

class DepthMap
{
public:
    explicit DepthMap();
    ~DepthMap();
    DepthMap(const DepthMap&) = delete;
    DepthMap(DepthMap&&) = delete;
    DepthMap& operator=(const DepthMap&) = delete;
    DepthMap& operator=(DepthMap&&) = delete;

    bool init(GLuint shader);
    void bind(const glm::vec3& lightPos, const glm::vec3& lightDir);
    void activate();

    void setWidth(int width);
    void setHeight(int height);

private:
    int width;
    int height;

    GLuint shader;

    GLuint framebuffer;
    GLuint depthTexture;

    glm::vec4 frustum;
    float nearClipDistance;
    float farClipDistance;
    glm::mat4 lightSpace;
};

} // moar

#endif // FRAMEBUFFER_H
