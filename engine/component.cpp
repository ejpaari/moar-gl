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

} // moar
