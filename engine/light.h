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

    void setColor(const glm::vec3& color);
    void setPower(float power);

    virtual std::string getName() final;
    virtual Component::Type getType() final;

private:
    static GLint shader;
    static GLuint lightBlockBuffer;
    static bool lightBlockBufferCreated;

    glm::vec3 color;
    float power;
};

} // moar

#endif // LIGHT_H
