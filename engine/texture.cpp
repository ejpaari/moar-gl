#include "texture.h"

#include <SOIL.h>
#include <iostream>

namespace moar
{

Texture::Texture()
{
    glGenTextures(1, &name);
}

Texture::~Texture()
{
    glDeleteTextures(1, &name);
}

bool Texture::load(const std::string& file)
{
    glBindTexture(GL_TEXTURE_2D, name);

    unsigned char* image;
    int width;
    int height;
    image = SOIL_load_image(file.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

    if (!image) {
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::cout << "Loaded texture: " << file << std::endl;

    return true;
}

} // moar
