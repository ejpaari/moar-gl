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
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(outputTextures.size(), &outputTextures[0]);
    glDeleteRenderbuffers(1, &depthRenderbuffer);
}

bool Framebuffer::init(int numOutputs)
{
    outputTextures.resize(numOutputs);
    glGenTextures(numOutputs, &outputTextures[0]);
    glGenFramebuffers(1, &framebuffer);
    glGenRenderbuffers(1, &depthRenderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    std::vector<GLenum> drawBuffers;
    for (int i = 0; i < numOutputs; ++i) {
        GLuint texture = outputTextures[i];
        GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
        drawBuffers.push_back(attachment);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB16F, width, height, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);
    }
    glDrawBuffers(numOutputs, &drawBuffers[0]);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::bind() const
{
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

GLuint Framebuffer::getFramebuffer() const
{
    return framebuffer;
}

std::vector<GLuint> Framebuffer::getOutputTextures() const
{
    return outputTextures;
}

} // moar
