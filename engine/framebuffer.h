#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>

#include <vector>

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

    bool init(int numOutputs);
    void bind() const;

    GLuint getFramebuffer() const;
    std::vector<GLuint> getOutputTextures() const;

private:
    static int width;
    static int height;

    GLuint framebuffer;
    GLuint renderedTexture = 0;
    GLuint depthRenderbuffer = 0;
    std::vector<GLuint> outputTextures;
};

} // moar

#endif // FRAMEBUFFER_H
