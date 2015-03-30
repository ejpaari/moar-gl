#include "object.h"

namespace moar
{

Object::Object() :
    forward(0.0f, 0.0f, 1.0f),
    up(0.0f, 1.0f, 0.0f)
{
}

Object::~Object()
{
}

} // moar
