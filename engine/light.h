#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace moar
{

class Light
{
    friend class Engine;

public:
    enum Type
    {
        POINT = 0,
        DIRECTIONAL = 1,
        NUM_TYPES = 2
    };

    explicit Light();
    ~Light();
    Light(const Light&) = delete;
    Light(Light&&) = delete;
    Light& operator=(const Light&) = delete;
    Light& operator=(Light&&) = delete;

    void setUniforms(const glm::vec3& position, const glm::vec3& forward);

    void setType(Type type);
    void setColor(const glm::vec4& color);
    void setShadowingEnabled(bool enabled);

    Type getLightType() const;
    bool isShadowingEnabled() const;

private:
    static GLuint lightBlockBuffer;

    Type type = POINT;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 10.0f);
    bool shadowingEnabled = true;
};

} // moar

#endif // LIGHT_H
