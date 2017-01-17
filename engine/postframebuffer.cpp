#include "postframebuffer.h"
#include "common/globals.h"

#include <iostream>

namespace moar
{

GLuint PostFramebuffer::quadVAO = 0;
GLuint PostFramebuffer::quadBuffer = 0;

void PostFramebuffer::initQuad()
{
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
}

void PostFramebuffer::uninitQuad()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadBuffer);
}

void PostFramebuffer::bindQuadVAO()
{
    glBindVertexArray(quadVAO);
}

PostFramebuffer::PostFramebuffer()
{
}

PostFramebuffer::~PostFramebuffer()
{
    deinit();
}

bool PostFramebuffer::init(GLsizei numOutputs, bool enableDepth)
{
    renderedTextures.resize(numOutputs);
    drawBufferAttachments.resize(numOutputs);
    glGenTextures(numOutputs, &renderedTextures[0]);
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    for (int i = 0; i < numOutputs; ++i) {
        GLuint texture = renderedTextures[i];
        GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
        drawBufferAttachments[i] = attachment;
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, 0);
    }
    glDrawBuffers(numOutputs, &drawBufferAttachments[0]);

    if (enableDepth) {
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        hasDepth = true;
    }

    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void PostFramebuffer::deinit()
{
	if (!renderedTextures.empty()) {
		glDeleteTextures(renderedTextures.size(), &renderedTextures[0]);
	}
    glDeleteFramebuffers(1, &framebuffer);    
    glDeleteRenderbuffers(1, &depthRenderbuffer);
}

GLuint PostFramebuffer::draw(const std::vector<GLuint>& textures)
{
    for (unsigned int i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glUniform1i(RENDERED_TEX_LOCATION0 + i, i);
    }
    glDrawBuffers(drawBufferAttachments.size(), &drawBufferAttachments[0]);

    bind();
    bindQuadVAO();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    return renderedTextures[0];
}

GLuint PostFramebuffer::blitColor(GLuint blitBuffer, int attachment) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, blitBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    return renderedTextures[0];
}

GLuint PostFramebuffer::getRenderedTex(unsigned int output) const
{
    if (output >= renderedTextures.size()) {
        std::cerr << "WARNING: Invalid output value for getting a rendered texture\n";
        return 0;
    }
    return renderedTextures[output];
}

} // moar
