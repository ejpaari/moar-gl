layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

layout (std140) uniform TransformationBlock {
  mat4 M;
  mat4 V;
  mat4 MV;
  mat4 MVP;
};

out vec2 texCoord;
out vec3 vertexPos_World;
out vec3 normal_World;

out mat3 TBN;

void main()
{
  gl_Position = MVP * vec4(position, 1.0);
  texCoord = tex;
  vertexPos_World = vec3(M * vec4(position, 1.0));
  normal_World = normalize(mat3(M) * normal);

#if defined(NORMAL)
  vec3 N = normal_World;
  vec3 T = normalize(mat3(M) * tangent);
  vec3 B = cross(T, N);
  TBN = mat3(T, B, N);
#endif

}
