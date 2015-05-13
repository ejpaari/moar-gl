#include "component.h"

namespace moar
{

Component::Component() :
    parent(nullptr)
{
}

Component::~Component()
{
}

void Component::setParent(Object* parentObject)
{
    parent = parentObject;
}

Object* Component::getParent() const
{
    return parent;
}

} // moar
