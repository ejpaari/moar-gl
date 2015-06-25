#include "constants.h"

namespace moar
{
const int TRANSFORMATION_BINDING_POINT = 1;
const int LIGHT_BINDING_POINT = 2;

const GLchar* TRANSFORMATION_BLOCK_NAME = "TransformationBlock";
const GLchar* LIGHT_BLOCK_NAME = "LightBlock";

const GLuint VERTEX_LOCATION = 1;
const GLuint TEX_LOCATION = 2;
const GLuint NORMAL_LOCATION = 3;
const GLuint TANGENT_LOCATION = 4;
const GLuint AMBIENT_LOCATION = 10;
const GLuint SPECULAR_LOCATION = 11;

bool DEBUG = false;
} // moar
