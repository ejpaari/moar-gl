#ifndef TYPEMAPPINGS
#define TYPEMAPPINGS

#include "globals.h"
#include "../material.h"
#include "../shader.h"
#include "../light.h"

#include <assimp/scene.h>

#include <string>
#include <vector>
#include <map>

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
extern const std::map<Light::Type, std::string> lightDefineMappings;

} // moar

#endif // TYPEMAPPINGS

