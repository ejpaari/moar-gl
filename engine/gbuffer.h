#ifndef GBUFFER_H
#define GBUFFER_H

#include "framebuffer.h"

#include <GL/glew.h>

#include <vector>

namespace moar
{

class GBuffer : public Framebuffer
{
public:
    explicit GBuffer();
    ~GBuffer();
    GBuffer(const GBuffer&) = delete;
    GBuffer(GBuffer&&) = delete;
    GBuffer& operator=(const GBuffer&) = delete;
    GBuffer& operator=(GBuffer&&) = delete;

    bool init();

    GLuint getPositionTexture() const;
    GLuint getNormalTexture() const;
    GLuint getColorTexture() const;
    std::vector<GLuint> getTextures() const;

private:
    GLuint positionTexture = 0;
    GLuint normalTexture = 0;
    GLuint colorTexture = 0;
    GLuint depthRenderbuffer = 0;
};

} // moar

#endif // GBUFFER_H
