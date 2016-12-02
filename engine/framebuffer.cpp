#include "framebuffer.h"

namespace moar
{

int Framebuffer::width = 800;
int Framebuffer::height = 600;

void Framebuffer::setSize(int width, int height)
{
    Framebuffer::width = width;
    Framebuffer::height = height;
}

Framebuffer::Framebuffer()
{
}

Framebuffer::~Framebuffer()
{
}

void Framebuffer::bind() const
{
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

} // moar
