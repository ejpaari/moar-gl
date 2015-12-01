#include "component.h"

namespace moar
{

const Shader* Component::shader = nullptr;

Component::Component() :
    parent(nullptr),
    enabled(true)
{
}

Component::~Component()
{
}

void Component::setShader(const Shader* shader)
{
    Component::shader = shader;
}

void Component::setParent(Object* parentObject)
{
    parent = parentObject;
}

void Component::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

bool Component::isEnabled() const
{
    return enabled;
}

} // moar
