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
    enum TextureType {
        DIFFUSE,
        NORMAL,
        DISPLACEMENT
    };

    struct TextureInfo {
        TextureType type;
        const GLchar* name;
        int unit;
        int value;
    };

    static const TextureInfo textureInfos[];

    Material();
    virtual ~Material();
    Material(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(const Material&) = delete;
    Material& operator=(Material&&) = delete;

    virtual void execute() final;

    void setShader(GLuint shader) { this->shader = shader; }
    void setTexture(GLuint texture, TextureType type);

    virtual std::string getName() final { return "Material"; }
    virtual Component::Type getType() final { return Component::Type::MATERIAL; }

private:
    const TextureInfo* getTextureInfo(TextureType type);

    GLuint shader;
    std::vector<std::tuple<GLuint, const TextureInfo*>> textures;
};

} // moar

#endif // MATERIAL_H
