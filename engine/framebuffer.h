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

    void bind() const;

protected:
    static int width;
    static int height;

    GLuint framebuffer;
};

} // moar

#endif // FRAMEBUFFER_H
