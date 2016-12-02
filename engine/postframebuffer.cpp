#include "postframebuffer.h"
#include "common/globals.h"

namespace moar
{

PostFramebuffer::PostFramebuffer()
{
}

PostFramebuffer::~PostFramebuffer()
{
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &renderedTexture);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadBuffer);
}

bool PostFramebuffer::init()
{
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

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

void PostFramebuffer::activate()
{
    for (unsigned int i = 0; i < inputTextures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, inputTextures[i]);
        glUniform1i(RENDERED_TEX_LOCATION0 + i, i);
    }
    glBindVertexArray(quadVAO);
}

GLuint PostFramebuffer::draw(const std::vector<GLuint>& textures)
{
    bind();
    setInputTextures(textures);
    activate();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    return renderedTexture;
}

GLuint PostFramebuffer::blit(GLuint blitBuffer, int attachment) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, blitBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    return renderedTexture;
}

void PostFramebuffer::setInputTextures(const std::vector<GLuint>& textures)
{
    inputTextures = textures;
}

GLuint PostFramebuffer::getFramebuffer() const
{
    return framebuffer;
}

} // moar
