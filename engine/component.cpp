#include "component.h"

namespace moar
{

Component::Component() :
    parent(nullptr),
    enabled(true)
{
}

Component::~Component()
{
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
