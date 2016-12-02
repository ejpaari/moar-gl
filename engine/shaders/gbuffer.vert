layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

layout (std140) uniform TransformationBlock {
  mat4 M;
  mat4 V;
  mat4 MV;
  mat4 MVP;
};

out vec2 texCoord;
out vec3 vertexPos_World;
out vec3 normal_World;

void main()
{
  gl_Position = MVP * vec4(position, 1.0);
  texCoord = tex;
  vertexPos_World = vec3(M * vec4(position, 1.0));
  normal_World = normalize(vec3(M * vec4(normal, 0.0)));
}
