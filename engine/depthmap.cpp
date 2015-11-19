#include "depthmap.h"

#include <iostream>

namespace moar
{

DepthMap::DepthMap() :
    width(0),
    height(0),
    nearClipDistance(0.1f),
    farClipDistance(100.0f),
    shader(0)
{
}

DepthMap::~DepthMap()
{
}

void DepthMap::setWidth(int width)
{
    this->width = width;
}

void DepthMap::setHeight(int height)
{
    this->height = height;
}

bool DepthMap::createFramebuffer(GLuint& framebuffer, GLuint& texture, bool cube)
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    cube
            ? glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0)
            : glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        return true;
    } else {
        std::cerr << "ERROR: Depth map framebuffer status is incomplete." << std::endl;
        return false;
    }
}

} // moar
