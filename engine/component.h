#ifndef COMPONENT_H
#define COMPONENT_H

#include "shader.h"

#include <string>

namespace moar
{

class Object;

class Component
{
public:
    enum Type
    {
        RENDERER,
        MATERIAL,
        LIGHT,
        CAMERA,
        CUSTOM
    };

    Component();
    virtual ~Component();
    Component(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(const Component&) = delete;
    Component& operator=(Component&&) = delete;

    virtual void execute() = 0;

    static void setShader(const Shader* shader);
    void setParent(Object* parentObject);
    void setEnabled(bool enabled);

    virtual std::string getName() = 0;
    virtual Component::Type getType() = 0;
    bool isEnabled() const;

protected:
    static const Shader* shader;

    Object* parent;
    bool enabled;
};

} // moar

#endif // COMPONENT_H
