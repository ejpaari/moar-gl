#include "framebuffer.h"
#include "common/globals.h"

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
    glDeleteTextures(1, &renderedTexture);
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadBuffer);
}

bool Framebuffer::init(bool multisample)
{
    glGenTextures(1, &renderedTexture);
    glGenFramebuffers(1, &framebuffer);
    glGenRenderbuffers(1, &depthRenderbuffer);

    if (multisample) {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderedTexture);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, renderedTexture, 0);
        GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers);

        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    } else {
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
        GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers);

        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    }

    GLfloat quadData[] = {
        -1.0f, -1.0f, 0.0f,   1.0f, -1.0f, 0.0f,   -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,   1.0f, -1.0f, 0.0f,    1.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);

    glVertexAttribPointer(VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_LOCATION);

    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::activate()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, previousFrame);
    glUniform1i(RENDERED_TEX_LOCATION, 0);
    glBindVertexArray(quadVAO);
}

void Framebuffer::bind() const
{
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::blit(GLuint blitBuffer) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, blitBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Framebuffer::setPreviousFrame(GLuint texture)
{
    previousFrame = texture;
}

GLuint Framebuffer::getRenderedTexture() const
{
    return renderedTexture;
}

GLuint Framebuffer::getFramebuffer() const
{
    return framebuffer;
}

} // moar
