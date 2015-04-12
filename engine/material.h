#ifndef MATERIAL_H
#define MATERIAL_H

#include "component.h"

#include <GL/glew.h>
#include <vector>
#include <string>

namespace moar
{

class Material : public Component
{
public:
    Material();
    virtual ~Material();
    Material(const Material&) = delete;
    Material(Material&&) = delete;
    Material& operator=(const Material&) = delete;
    Material& operator=(Material&&) = delete;

    virtual void execute() final;

    void setShader(GLuint shader) { this->shader = shader; }
    // Todo: specify texture types
    void setTexture(GLuint texture) { textures.push_back(texture); }

    std::string getType() final { return "Material"; }

private:
    GLuint shader;
    std::vector<GLuint> textures;
};

} // moar

#endif // MATERIAL_H
