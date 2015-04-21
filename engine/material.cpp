#include "material.h"

#include <iostream>

namespace moar
{

const Material::TextureInfo Material::textureInfos[] =
{
    {Material::TextureType::DIFFUSE, "DiffuseTex", GL_TEXTURE0, 0},
    {Material::TextureType::NORMAL, "NormalTex", GL_TEXTURE1, 1},
    {Material::TextureType::DISPLACEMENT, "DisplacementTex", GL_TEXTURE2, 2},
};

Material::Material()
{
}

Material::~Material()
{
}

void Material::execute()
{
    glUseProgram(shader);
    for (unsigned int i = 0; i < textures.size(); ++i) {
        glActiveTexture(std::get<1>(textures[i])->unit);
        glBindTexture(GL_TEXTURE_2D, std::get<0>(textures[i]));
        glUniform1i(glGetUniformLocation(shader, std::get<1>(textures[i])->name),
                    std::get<1>(textures[i])->value);
    }
}

void Material::setShader(GLuint shader)
{
    this->shader = shader;
}

void Material::setTexture(GLuint texture, TextureType type)
{
    for (unsigned int i = 0; i < textures.size(); ++i) {
        if (std::get<1>(textures[i])->type == type) {
            std::get<0>(textures[i]) = texture;
            return;
        }
    }

    textures.push_back(std::make_tuple(texture, getTextureInfo(type)));
}

std::string Material::getName()
{
    return "Material";
}

Component::Type Material::getType()
{
    return Component::Type::MATERIAL;
}

const Material::TextureInfo* Material::getTextureInfo(TextureType type)
{
    for (unsigned int i = 0; i < (sizeof(textureInfos)/sizeof(*textureInfos)); ++i) {
        if (textureInfos[i].type == type) {
            return &textureInfos[i];
        }
    }

    std::cerr << "Warning: Could not map texture information." << std::endl;
    return nullptr;
}

} // moar
