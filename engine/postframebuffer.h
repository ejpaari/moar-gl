#ifndef POSTFRAMEBUFFER_H
#define POSTFRAMEBUFFER_H

#include "framebuffer.h"

#include <GL/glew.h>

#include <vector>

namespace moar
{

class PostFramebuffer : public Framebuffer
{
public:
    static void setSize(int width, int height);

    explicit PostFramebuffer();
    ~PostFramebuffer();
    PostFramebuffer(const PostFramebuffer&) = delete;
    PostFramebuffer(PostFramebuffer&&) = delete;
    PostFramebuffer& operator=(const PostFramebuffer&) = delete;
    PostFramebuffer& operator=(PostFramebuffer&&) = delete;

    bool init();
    void activate();
    void bindVAO();
    GLuint draw(const std::vector<GLuint>& textures);
    GLuint blit(GLuint blitBuffer, int attachment) const;

    void setInputTextures(const std::vector<GLuint>& textures);
    GLuint getFramebuffer() const;
    GLuint getRenderedTexture() const;

private:    
    GLuint renderedTexture = 0;
    GLuint quadVAO = 0;
    GLuint quadBuffer = 0;
    std::vector<GLuint> inputTextures;
};

} // moar

#endif // POSTFRAMEBUFFER_H
