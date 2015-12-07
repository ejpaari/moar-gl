#include "component.h"

namespace moar
{

const Shader* Component::shader = nullptr;
bool Component::updateRequired = false;

Component::Component() :
    parent(nullptr),
    enabled(true)
{
    updateRequired = true;
}

Component::~Component()
{
    updateRequired = true;
}

void Component::setShader(const Shader* shader)
{
    Component::shader = shader;
    updateRequired = true;
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
