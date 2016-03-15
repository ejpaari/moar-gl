#ifndef GLOBALS_H
#define GLOBALS_H

#include <GL/glew.h>

#include <string>

namespace moar
{
extern const std::string GLSL_VERSION;
extern const std::string POINT_DEFINE;
extern const std::string DIRECTIONAL_DEFINE;
extern const std::string DIFFUSE_DEFINE;
extern const std::string SPECULAR_DEFINE;
extern const std::string NORMAL_DEFINE;
extern const std::string BUMP_DEFINE;

extern const std::string LIGHT_SHADER;

extern const int TRANSFORMATION_BINDING_POINT;
extern const int LIGHT_BINDING_POINT;

extern const GLchar* TRANSFORMATION_BLOCK_NAME;
extern const GLchar* LIGHT_BLOCK_NAME;

extern const GLuint VERTEX_LOCATION;
extern const GLuint TEX_LOCATION;
extern const GLuint NORMAL_LOCATION;
extern const GLuint TANGENT_LOCATION;

extern const GLuint AMBIENT_LOCATION;
extern const GLuint SPECULAR_LOCATION;
extern const GLuint SOLID_COLOR_LOCATION;

extern const GLuint DIFFUSE_TEX_LOCATION;
extern const GLuint NORMAL_TEX_LOCATION;
extern const GLuint BUMP_TEX_LOCATION;
extern const GLuint DEPTH_TEX_LOCATION;
extern const GLuint SPEC_TEX_LOCATION;

extern const GLuint RENDERED_TEX_LOCATION;

extern const GLuint TIME_LOCATION;
extern const GLuint SCREEN_SIZE_LOCATION;
extern const GLuint RECEIVE_SHADOWS_LOCATION;
extern const GLuint FAR_CLIP_DISTANCE_LOCATION;

extern const GLuint LIGHT_SPACE_PROJ_LOCATION;
extern const GLuint LIGHT_SPACE_VP_LOCATION;

const GLint MAX_LOCATION = 64;

extern unsigned int G_DRAW_COUNT;
extern bool G_COMPONENT_CHANGED;
} // moar

#endif // GLOBALS_H

