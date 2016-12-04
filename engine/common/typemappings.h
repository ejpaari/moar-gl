#ifndef TYPEMAPPINGS
#define TYPEMAPPINGS

#include "globals.h"
#include "../material.h"
#include "../shader.h"

#include <assimp/scene.h>

#include <string>
#include <vector>

namespace moar
{

struct TypeMapping
{
    aiTextureType aiType;
    Material::TextureType materialType;
    Shader::Type shaderType;
    std::string shaderDefine;
};

extern const std::vector<TypeMapping> textureTypeMappings;

} // moar

#endif // TYPEMAPPINGS

