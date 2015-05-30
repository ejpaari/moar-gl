#include "texture.h"

#include <SOIL.h>
#include <iostream>

namespace moar
{

Texture::Texture()
{
    glGenTextures(1, &id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

bool Texture::load(const std::string& file)
{
    glBindTexture(GL_TEXTURE_2D, id);

    int width;
    int height;
    unsigned char* image = SOIL_load_image(file.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

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

bool Texture::load(const std::vector<std::string>& files)
{
    if (files.size() != 6) {
        std::cerr << "Could not load cube map texture, requires six faces." << std::endl;
        return false;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width;
    int height;
    for(unsigned int i = 0; i < files.size(); i++) {
        unsigned char* image = SOIL_load_image(files[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        if (!image) {
            std::cerr << "Failed to load cube texture: " << files[i] << std::endl;
            return false;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout << "Loaded cube map textures:";
    for (auto i : files) {
        std::cout << " " << i;
    }
    std::cout << std::endl;

    return true;
}

GLuint Texture::getID() const
{
    return id;
}

} // moar
