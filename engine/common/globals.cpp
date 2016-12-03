#include "globals.h"

namespace moar
{
const std::string GLSL_VERSION = "#version 440 core\n";
const std::string POINT_DEFINE = "#define POINT\n";
const std::string DIRECTIONAL_DEFINE = "#define DIRECTIONAL\n";
const std::string DIFFUSE_DEFINE = "#define DIFFUSE\n";
const std::string SPECULAR_DEFINE = "#define SPECULAR\n";
const std::string NORMAL_DEFINE = "#define NORMAL\n";
const std::string BUMP_DEFINE = "#define BUMP\n";

const std::string LIGHT_SHADER = "light";

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

const GLuint DIFFUSE_TEX_LOCATION = 20;
const GLuint NORMAL_TEX_LOCATION = 21;
const GLuint BUMP_TEX_LOCATION = 22;
const GLuint DEPTH_TEX_LOCATION = 23;
const GLuint SPEC_TEX_LOCATION = 24;

const GLuint RENDERED_TEX_LOCATION0 = 30;
const GLuint RENDERED_TEX_LOCATION1 = 31;
const GLuint RENDERED_TEX_LOCATION2 = 32;
const GLuint RENDERED_TEX_LOCATION3 = 33;

const GLuint TIME_LOCATION = 40;
const GLuint SCREEN_SIZE_LOCATION = 41;
const GLuint RECEIVE_SHADOWS_LOCATION = 42;
const GLuint FAR_CLIP_DISTANCE_LOCATION = 43;
const GLuint BLOOM_FILTER_HORIZONTAL = 44;

const GLuint LIGHT_SPACE_PROJ_LOCATION = 50;
const GLuint LIGHT_SPACE_VP_LOCATION = 51;

unsigned int G_DRAW_COUNT = 0;
bool G_COMPONENT_CHANGED = false;
} // moar
