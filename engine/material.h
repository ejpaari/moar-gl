#ifndef MATERIAL_H
#define MATERIAL_H

#include "component.h"

#include <GL/glew.h>
#include <string>
#include <tuple>
#include <vector>
#include <functional>
#include <unordered_map>

namespace moar
{

class Material : public Component
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
    virtual ~Material();
    Material(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(const Material&) = delete;
    Material& operator=(Material&&) = delete;

    virtual void execute() final;

    void setShaderType(const std::string& shaderType);
    void setTexture(GLuint texture, TextureType type, GLenum target);
    // Todo: Duplicate location info.
    void setUniform(const std::string& name, std::function<void()> func, GLuint location);

    virtual std::string getName() final;
    virtual Component::Type getType() final;
    std::string getShaderType() const;

private:
    struct CustomUniform
    {
        GLuint location;
        std::function<void()> func;
    };

    const TextureInfo* getTextureInfo(TextureType type);

    std::string shaderType;
    std::vector<std::tuple<GLuint, const TextureInfo*, GLenum>> textures;
    std::unordered_map<std::string, CustomUniform> uniforms;
};

} // moar

#endif // MATERIAL_H
