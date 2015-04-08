#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "object.h"
#include "model.h"

#include <string>

namespace moar
{

class RenderObject : public Object
{
public:
    static const glm::mat4* projection;
    static const glm::mat4* view;

    explicit RenderObject();
    ~RenderObject();
    RenderObject(const RenderObject&) = delete;
    RenderObject(RenderObject&&) = delete;
    RenderObject& operator=(const RenderObject&) = delete;
    RenderObject& operator=(RenderObject&&) = delete;

    bool init(GLuint shaderProgram, Model* renderModel, GLuint textureName);
    void render();

private:
    Model* model;
    GLuint shader;
    GLuint texture;
};

} // moar

#endif // RENDEROBJECT_H
