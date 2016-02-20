#include "depthmap.h"

#include <iostream>

namespace moar
{

DepthMap::DepthMap()
{
}

DepthMap::~DepthMap()
{
}

void DepthMap::setSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

bool DepthMap::createFramebuffer(GLuint& framebuffer, GLuint& texture, bool cube)
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    cube ? glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0)
         : glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        return true;
    } else {
        std::cerr << "ERROR: Depth map framebuffer status is incomplete.\n";
        return false;
    }
}

} // moar
