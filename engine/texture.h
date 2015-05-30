#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <vector>

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
    bool load(const std::vector<std::string>& files);

    GLuint getID() const;

private:
    GLuint id;
};

} // moar

#endif // TEXTURE_H
