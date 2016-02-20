#include "texture.h"

#include <SOIL.h>
#include <iostream>
#include <algorithm>

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
    int channels;
    unsigned char* image = SOIL_load_image(file.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);

    if (!image) {
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::cout << "Loaded texture: " << file << "\n";

    return true;
}

bool Texture::load(const std::vector<std::string>& files)
{
    if (files.size() != 6) {
        std::cerr << "WARNING: Could not load cube map texture, requires six faces\n";
        return false;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width;
    int height;
    for(unsigned int i = 0; i < files.size(); i++) {
        unsigned char* image = SOIL_load_image(files[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        if (!image) {
            std::cerr << "WARNING: Failed to load cube texture; " << files[i] << "\n";
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
    std::for_each(files.begin(), files.end(), [] (const std::string& s) { std::cout << " " << s; });
    std::cout << "\n";

    return true;
}

GLuint Texture::getID() const
{
    return id;
}

} // moar
