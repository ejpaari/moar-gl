#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"

#include <GL/glew.h>

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace moar
{

class Material
{
    friend class Renderer;

public:
    enum TextureType
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        BUMP
    };

    Material();
    ~Material();
    Material(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(const Material&) = delete;
    Material& operator=(Material&&) = delete;

    void setShaderType(int shaderType);
    void setTexture(GLuint texture, TextureType type, GLenum target);
    void setCustomUniform(const std::string& name, std::function<void()> func, GLuint location);

    int getShaderType() const;
    int getId() const;
    unsigned int getNumTextures() const;

    void checkMissingTextures() const;

private:
    struct CustomUniform
    {
        GLuint location;
        std::function<void()> func;
    };

    struct TextureInfo
    {
        TextureType type;
        const GLchar* name;
        int unit;
        int value;
        GLuint location;
    };

    struct MaterialTexture
    {
        GLuint glId;
        const TextureInfo* info;
        GLenum target;
    };

    static const TextureInfo textureInfos[];
    static int idCounter;

    void setUniforms(const Shader* shader);
    const TextureInfo* getTextureInfo(TextureType type);

    int shaderType = Shader::UNDEFINED;
    std::vector<MaterialTexture> textures;
    std::unordered_map<std::string, CustomUniform> uniforms;

    int id;
};

} // moar

#endif // MATERIAL_H
