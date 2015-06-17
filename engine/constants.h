#ifndef CONSTANTS
#define CONSTANTS

#include <GL/glew.h>

namespace moar
{
extern const int TRANSFORMATION_BINDING_POINT;
extern const int LIGHT_BINDING_POINT;

extern const GLchar* TRANSFORMATION_BLOCK_NAME;
extern const GLchar* LIGHT_BLOCK_NAME;

extern const GLuint VERTEX_LOCATION;
extern const GLuint TEX_LOCATION;
extern const GLuint NORMAL_LOCATION;
extern const GLuint AMBIENT_LOCATION;
extern const GLuint SPECULAR_LOCATION;

extern bool DEBUG;
} // moar

#endif // CONSTANTS

