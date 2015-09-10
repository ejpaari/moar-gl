#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>

namespace moar
{

class Framebuffer
{
public:
    explicit Framebuffer();
    ~Framebuffer();
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    bool init();
    void activate();
    void bind() const;

    void setPreviousFrame(GLuint texture);
    GLuint getRenderedTexture() const;

    static void setWidth(int width);
    static void setHeight(int height);

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
