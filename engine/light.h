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
    // Todo: Directional light.

    enum Type
    {
        POINT = 0,
        DIRECTIONAL = 1,
        NUM_TYPES = 2
        // Todo: Spotlight.
    };

    explicit Light(Type type);
    virtual ~Light();
    Light(const Light&) = delete;
    Light(Light&&) = delete;
    Light& operator=(const Light&) = delete;
    Light& operator=(Light&&) = delete;

    virtual void execute() final;

    void setColor(const glm::vec4& color);
    void setDirection(const glm::vec3& direction);

    Type getLightType() const;
    virtual std::string getName() final;
    virtual Component::Type getType() final;

private:
    GLuint lightBlockBuffer;
    Type type;
    glm::vec4 color;
    glm::vec3 direction;
};

} // moar

#endif // LIGHT_H
