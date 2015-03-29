#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "object.h"
#include "model.h"

#include <string>

namespace moar {

class RenderObject : public Object
{
public:
    RenderObject();
    ~RenderObject();
    RenderObject(const RenderObject&) = delete;
    RenderObject(RenderObject&&) = delete;
    RenderObject& operator=(const RenderObject&) = delete;
    RenderObject& operator=(RenderObject&&) = delete;

    bool init(GLuint shaderProgram, const std::string& modelName);
    void render();

private:
    Model model;
    GLuint shader;
    GLuint textures[1];

    GLint uniView;
    GLint uniProj;
};

} // moar

#endif // RENDEROBJECT_H
