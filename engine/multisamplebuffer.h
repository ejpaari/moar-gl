#ifndef MULTISAMPLEBUFFER_H
#define MULTISAMPLEBUFFER_H

#include "framebuffer.h"

#include <GL/glew.h>

#include <vector>

namespace moar
{

class MultisampleBuffer : public Framebuffer
{

public:
    explicit MultisampleBuffer();
    ~MultisampleBuffer();
    MultisampleBuffer(const MultisampleBuffer&) = delete;
    MultisampleBuffer(MultisampleBuffer&&) = delete;
    MultisampleBuffer& operator=(const MultisampleBuffer&) = delete;
    MultisampleBuffer& operator=(MultisampleBuffer&&) = delete;

    bool init(int numOutputs);

    GLuint getFramebuffer() const;
    std::vector<GLuint> getOutputTextures() const;

private:
    std::vector<GLuint> outputTextures;
    GLuint depthRenderbuffer = 0;
};

} // moar

#endif // MULTISAMPLEBUFFER_H
