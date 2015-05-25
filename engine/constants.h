#ifndef CONSTANTS
#define CONSTANTS

#include <GL/glew.h>

namespace moar
{
static const int TRANSFORMATION_BINDING_POINT = 1;
static const int LIGHT_BINDING_POINT = 2;

static const GLchar* TRANSFORMATION_BLOCK_NAME = "TransformationBlock";
static const GLchar* LIGHT_BLOCK_NAME = "LightBlock";

static const GLuint VERTEX_LOCATION = 1;
static const GLuint TEX_LOCATION = 2;
static const GLuint NORMAL_LOCATION = 3;
static const GLuint AMBIENT_LOCATION = 10;
} // moar

#endif // CONSTANTS

