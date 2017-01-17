#ifndef POSTFRAMEBUFFER_H
#define POSTFRAMEBUFFER_H

#include "framebuffer.h"

#include <GL/glew.h>

#include <vector>

namespace moar
{

class PostFramebuffer : public Framebuffer
{
public:
    static void initQuad();
    static void uninitQuad();
    static void bindQuadVAO();

    explicit PostFramebuffer();
    ~PostFramebuffer();
    PostFramebuffer(const PostFramebuffer&) = delete;
    PostFramebuffer(PostFramebuffer&&) = delete;
    PostFramebuffer& operator=(const PostFramebuffer&) = delete;
    PostFramebuffer& operator=(PostFramebuffer&&) = delete;

    bool init(GLsizei numOutputs, bool enableDepth);
    void deinit();
    GLuint draw(const std::vector<GLuint>& textures);
    GLuint blitColor(GLuint blitBuffer, int attachment) const;

    GLuint getRenderedTex(unsigned int output) const;

private:
    static GLuint quadVAO;
    static GLuint quadBuffer;

    std::vector<GLuint> renderedTextures;
    std::vector<GLenum> drawBufferAttachments;
    GLuint depthRenderbuffer = 0;

    bool hasDepth = false;
};

} // moar

#endif // POSTFRAMEBUFFER_H
