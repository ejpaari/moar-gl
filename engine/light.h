#ifndef LIGHT_H
#define LIGHT_H

#include "component.h"

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace moar
{

class Light : public Component
{
public:
    explicit Light();
    virtual ~Light();
    Light(const Light&) = delete;
    Light(Light&&) = delete;
    Light& operator=(const Light&) = delete;
    Light& operator=(Light&&) = delete;

    virtual void execute() final;

    void setColor(const glm::vec4& color);

    virtual std::string getName() final;
    virtual Component::Type getType() final;

private:
    GLuint lightBlockBuffer;
    glm::vec4 color;
};

} // moar

#endif // LIGHT_H
