#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"

#include <GL/glew.h>
#include <string>
#include <tuple>
#include <vector>
#include <functional>
#include <unordered_map>

namespace moar
{

class Material
{
public:
    enum TextureType
    {
        DIFFUSE,
        NORMAL,
        DISPLACEMENT
    };

    struct TextureInfo
    {
        TextureType type;
        const GLchar* name;
        int unit;
        int value;
        GLuint location;
    };

    static const TextureInfo textureInfos[];

    Material();
    ~Material();
    Material(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(const Material&) = delete;
    Material& operator=(Material&&) = delete;

    void setMaterialUniforms(const Shader* shader);

    void setShaderType(const std::string& shaderType);
    void setTexture(GLuint texture, TextureType type, GLenum target);
    void setUniform(const std::string& name, std::function<void()> func, GLuint location);

    std::string getShaderType() const;
    int getId() const;

private:
    struct CustomUniform
    {
        GLuint location;
        std::function<void()> func;
    };

    const TextureInfo* getTextureInfo(TextureType type);

    static int idCounter;

    std::string shaderType;
    std::vector<std::tuple<GLuint, const TextureInfo*, GLenum>> textures;
    std::unordered_map<std::string, CustomUniform> uniforms;

    int id;
};

} // moar

#endif // MATERIAL_H
