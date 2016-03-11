#include "material.h"
#include "common/globals.h"

#include <iostream>

namespace moar
{

int Material::idCounter = 0;

const Material::TextureInfo Material::textureInfos[] =
{
    {Material::TextureType::DIFFUSE, "diffuseTex", GL_TEXTURE1, 1, DIFFUSE_TEX_LOCATION},
    {Material::TextureType::NORMAL, "normalTex", GL_TEXTURE2, 2, NORMAL_TEX_LOCATION},
    {Material::TextureType::BUMP, "bumpTex", GL_TEXTURE3, 3, BUMP_TEX_LOCATION},
};

Material::Material()
{
    id = ++idCounter;
}

Material::~Material()
{
}

void Material::setShaderType(int shaderType)
{
    G_COMPONENT_CHANGED = true;
    this->shaderType = shaderType;    
}

void Material::setTexture(GLuint texture, TextureType type, GLenum target)
{
    // Todo: Reset textures if not set.
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

int Material::getShaderType() const
{
    return shaderType;
}

int Material::getId() const
{
    return id;
}

void Material::setUniforms(const Shader* shader)
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

const Material::TextureInfo* Material::getTextureInfo(TextureType type)
{
    for (unsigned int i = 0; i < (sizeof(textureInfos)/sizeof(*textureInfos)); ++i) {
        if (textureInfos[i].type == type) {
            return &textureInfos[i];
        }
    }

    std::cerr << "WARNING: Could not map material texture information\n";
    return nullptr;
}

} // moar
