layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

layout (location = 12) uniform vec3 cameraPos_World;
layout (location = 50) uniform mat4 lightSpaceProj;

layout (std140) uniform TransformationBlock {
  mat4 M;
  mat4 V;
  mat4 MV;
  mat4 MVP;
};

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

out vec2 texCoord;

out vec3 vertexPos_World;
out vec4 pos_Light;

out vec3 pointLightDir_World;
out vec3 normal_World;

out mat3 TBN;
out vec3 T;
out vec3 B;

out vec3 eyeDir_World;

void main()
{
  gl_Position = MVP * vec4(position, 1.0);
  texCoord = tex;
  vertexPos_World = vec3(M * vec4(position, 1.0));
  
#if defined(DIRECTIONAL)
  pos_Light = lightSpaceProj * M * vec4(position, 1.0);
#endif

normal_World = normalize(mat3(M) * normal);
#if defined(POINT)
  pointLightDir_World = lightPos - vertexPos_World;
#endif

#if defined(BUMP) || defined(NORMAL)
  vec3 N = normal_World;
  T = normalize(mat3(M) * tangent);
  B = cross(T, N);
  TBN = mat3(T, B, N);
#endif

#if defined(BUMP) || defined(SPECULAR)
  eyeDir_World = cameraPos_World - vertexPos_World;
#endif
}
