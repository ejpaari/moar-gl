#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

namespace moar
{

class Texture
{
public:
    explicit Texture();
    ~Texture();
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    bool load(const std::string& file);

    GLuint getName() const { return name; }

private:
    GLuint name;
};

} // moar

#endif // TEXTURE_H
