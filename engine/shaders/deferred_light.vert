layout (location = 1) in vec3 position;

layout (std140) uniform TransformationBlock {
  mat4 M;
  mat4 V;
  mat4 MV;
  mat4 MVP;
};

out vec2 texCoord;

void main()
{
#if defined(POINT)
  gl_Position = MVP * vec4(position, 1.0);
#else
  texCoord = (position.xy + vec2(1,1)) / 2.0;
  gl_Position = vec4(position, 1.0);
#endif
}
