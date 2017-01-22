#ifndef GLOBALS_H
#define GLOBALS_H

#include <GL/glew.h>

#include <string>

namespace moar
{

extern const std::string GLSL_VERSION;
extern const std::string COMMON_VERTEX_FILE;
extern const std::string COMMON_FRAGMENT_FILE;
extern const std::string POINT_DEFINE;
extern const std::string DIRECTIONAL_DEFINE;
extern const std::string DIFFUSE_DEFINE;
extern const std::string SPECULAR_DEFINE;
extern const std::string NORMAL_DEFINE;
extern const std::string BUMP_DEFINE;

extern const std::string FORWARD_LIGHT_SHADER;
extern const std::string DEFERRED_LIGHT_SHADER;
extern const std::string GBUFFER_SHADER;

extern const int TRANSFORMATION_BINDING_POINT;
extern const int LIGHT_BINDING_POINT;

extern const GLchar* TRANSFORMATION_BLOCK_NAME;
extern const GLchar* LIGHT_BLOCK_NAME;

extern const GLuint VERTEX_LOCATION;
extern const GLuint TEX_LOCATION;
extern const GLuint NORMAL_LOCATION;
extern const GLuint TANGENT_LOCATION;

extern const GLuint AMBIENT_LOCATION;
extern const GLuint CAMERA_POS_LOCATION;
extern const GLuint LIGHT_COLOR_LOCATION;
extern const GLuint LIGHT_POS_LOCATION;
extern const GLuint LIGHT_FORWARD_LOCATION;
extern const GLuint NUM_LIGHTS_LOCATION;

extern const GLuint DIFFUSE_TEX_LOCATION;
extern const GLuint NORMAL_TEX_LOCATION;
extern const GLuint BUMP_TEX_LOCATION;
extern const GLuint SPEC_TEX_LOCATION;
extern const GLuint DEPTH_TEX_LOCATION;

extern const GLuint RENDERED_TEX_LOCATION0;
extern const GLuint RENDERED_TEX_LOCATION1;
extern const GLuint RENDERED_TEX_LOCATION2;
extern const GLuint RENDERED_TEX_LOCATION3;

extern const GLuint TIME_LOCATION;
extern const GLuint SCREEN_SIZE_LOCATION;
extern const GLuint ENABLE_SHADOW_LOCATION;
extern const GLuint FAR_CLIP_DISTANCE_LOCATION;
extern const GLuint BLOOM_FILTER_HORIZONTAL;
extern const GLuint PROJECTION_MATRIX_LOCATION;

extern const GLuint LIGHT_SPACE_PROJ_LOCATION;
extern const GLuint LIGHT_SPACE_VP_LOCATION;

extern const GLuint ENABLE_SHADOWS_LOCATION;
const int MAX_NUM_SHADOWMAPS = 2; // Ensure there are enough uniform locations

extern const GLuint SSAO_KERNEL_LOCATION;
const int SSAO_KERNEL_SIZE = 64; // Ensure there are enough uniform locations

const GLint MAX_UNIFORM_LOCATION = 256;

const int MAX_NUM_LIGHTS_PER_TYPE = 16;

extern unsigned int G_DRAW_COUNT;
extern bool G_COMPONENT_CHANGED;

} // moar

#endif // GLOBALS_H

