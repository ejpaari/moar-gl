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
    enum Type
    {
        POINT = 0,
        DIRECTIONAL = 1,
        NUM_TYPES = 2
        // Todo: Spotlight.
    };

    explicit Light();
    virtual ~Light();
    Light(const Light&) = delete;
    Light(Light&&) = delete;
    Light& operator=(const Light&) = delete;
    Light& operator=(Light&&) = delete;

    virtual void execute() final;

    void setType(Type type);
    void setColor(const glm::vec4& color);
    void setShadowingEnabled(bool enabled);

    Type getLightType() const;
    virtual std::string getName() final;
    virtual Component::Type getType() final;
    bool isShadowingEnabled() const;

private:
    static GLuint lightBlockBuffer;
    static bool bufferCreated;

    Type type = POINT;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 10.0f);
    bool shadowingEnabled = true;
};

} // moar

#endif // LIGHT_H
