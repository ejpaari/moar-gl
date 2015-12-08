#ifndef COMPONENT_H
#define COMPONENT_H

#include "shader.h"

#include <string>

namespace moar
{

class Object;

class Component
{
    friend class Object;

public:
    enum Type
    {
        RENDERER,
        MATERIAL,
        LIGHT,
        CAMERA
    };

    Component();
    virtual ~Component();
    Component(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(const Component&) = delete;
    Component& operator=(Component&&) = delete;

    void setEnabled(bool enabled);

    virtual std::string getName() = 0;
    virtual Component::Type getType() = 0;
    bool isEnabled() const;

protected:
    static const Shader* shader;
    static bool updateRequired;

    virtual void execute() = 0;

    void setParent(Object* parentObject);

    Object* parent;
    bool enabled;

private:
    static void setShader(const Shader* shader);
};

} // moar

#endif // COMPONENT_H
