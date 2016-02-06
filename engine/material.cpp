#include "material.h"
#include "common/globals.h"

#include <iostream>

namespace moar
{

int Material::idCounter = 0;

const Material::TextureInfo Material::textureInfos[] =
{
    {Material::TextureType::DIFFUSE, "DiffuseTex", GL_TEXTURE1, 1, DIFFUSE_TEX_LOCATION},
    {Material::TextureType::NORMAL, "NormalTex", GL_TEXTURE2, 2, NORMAL_TEX_LOCATION},
    {Material::TextureType::DISPLACEMENT, "DisplacementTex", GL_TEXTURE3, 3, DISPLACEMENT_TEX_LOCATION},
};

Material::Material()
{
    id = ++idCounter;
}

Material::~Material()
{
}

void Material::setMaterialUniforms(const Shader* shader)
{
    for (unsigned int i = 0; i < textures.size(); ++i) {
        if (shader->hasUniform(textures[i].info->location)) {
            glActiveTexture(textures[i].info->unit);
            glBindTexture(textures[i].target, textures[i].glId);
            glUniform1i(textures[i].info->location, textures[i].info->value);
        }
    }

    for (auto iter = uniforms.begin(); iter != uniforms.end(); ++iter) {
        if (shader->hasUniform(iter->second.location)) {
            iter->second.func();
        }
    }
}

void Material::setShaderType(const std::string& shaderType)
{
    COMPONENT_CHANGED = true;
    this->shaderType = shaderType;    
}

void Material::setTexture(GLuint texture, TextureType type, GLenum target)
{
    for (unsigned int i = 0; i < textures.size(); ++i) {
        if (textures[i].info->type == type) {
            textures[i].glId = texture;
            textures[i].target = target;
            return;
        }
    }

    MaterialTexture materialTex = {texture, getTextureInfo(type), target};
    textures.push_back(materialTex);
}

void Material::setUniform(const std::string& name, std::function<void ()> func, GLuint location)
{
    CustomUniform uniform;
    uniform.func = func;
    uniform.location = location;
    uniforms[name] = uniform;
}

const Material::TextureInfo* Material::getTextureInfo(TextureType type)
{
    for (unsigned int i = 0; i < (sizeof(textureInfos)/sizeof(*textureInfos)); ++i) {
        if (textureInfos[i].type == type) {
            return &textureInfos[i];
        }
    }

    std::cerr << "WARNING: Could not map material texture information" << std::endl;
    return nullptr;
}

std::string Material::getShaderType() const
{
    return shaderType;
}

int Material::getId() const
{
    return id;
}

} // moar
