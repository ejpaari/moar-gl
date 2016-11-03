#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>

namespace moar
{

class Framebuffer
{
public:
    static void setSize(int width, int height);

    explicit Framebuffer();
    ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    bool init(bool multisample);
    void activate();
    void bind() const;
    void blit(GLuint blitBuffer) const;

    void setPreviousFrame(GLuint texture);
    GLuint getRenderedTexture() const;
    GLuint getFramebuffer() const;

private:
    static int width;
    static int height;

    GLuint framebuffer;
    GLuint renderedTexture;
    GLuint depthRenderbuffer;
    GLuint quadVAO;
    GLuint quadBuffer;
    GLuint previousFrame;
};

} // moar

#endif // FRAMEBUFFER_H
