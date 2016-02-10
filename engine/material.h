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

    Material();
    ~Material();
    Material(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(const Material&) = delete;
    Material& operator=(Material&&) = delete;

    void setMaterialUniforms(const Shader* shader);

    void setShaderType(const std::string& shaderType);
    void setTexture(GLuint texture, TextureType type, GLenum target);
    // Todo: Wrapper function for setting uniforms, this is a bit clumsy.
    // Todo: Uniforms should be reset for each material, otherwise they "leak" from previous.
    void setUniform(const std::string& name, std::function<void()> func, GLuint location);

    std::string getShaderType() const;
    int getId() const;

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

    const TextureInfo* getTextureInfo(TextureType type);

    std::string shaderType;
    std::vector<MaterialTexture> textures;
    std::unordered_map<std::string, CustomUniform> uniforms;

    int id;
};

} // moar

#endif // MATERIAL_H
