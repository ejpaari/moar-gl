#ifndef MATERIAL_H
#define MATERIAL_H

#include "component.h"

#include <GL/glew.h>
#include <string>
#include <tuple>
#include <vector>

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
    void setShader(GLuint shader);
    void setTexture(GLuint texture, TextureType type, GLenum target);
    void setSpecularity(float specularity);

    virtual std::string getName() final;
    virtual Component::Type getType() final;
    std::string getShaderType() const;
    GLuint getShader() const;

private:
    const TextureInfo* getTextureInfo(TextureType type);

    std::string shaderType;
    GLuint shader;
    std::vector<std::tuple<GLuint, const TextureInfo*, GLenum>> textures;
    float specularity;
};

} // moar

#endif // MATERIAL_H
