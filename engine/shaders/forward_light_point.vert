layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

layout (location = 12) uniform vec3 cameraPos_World;
layout (location = 16) uniform int numLights;
layout (location = 50) uniform mat4 lightSpaceProj;

layout (std140) uniform TransformationBlock {
  mat4 M;
  mat4 V;
  mat4 MV;
  mat4 MVP;
  mat4 NormalMatrix;
};

layout (std140) uniform LightProjectionBlock {
  mat4 LP[MAX_NUM_LIGHTS_PER_TYPE];
};

out vec2 texCoord;
out vec3 vertexPos_World;
out vec3 normal_World;
out vec3 eyeDir_World;
out vec4 pos_Light[MAX_NUM_LIGHTS_PER_TYPE];
out vec3 T;
out vec3 B;
out mat3 TBN;

void main()
{
  gl_Position = MVP * vec4(position, 1.0);
  texCoord = tex;
  vertexPos_World = vec3(M * vec4(position, 1.0));
  normal_World = normalize(vec3(NormalMatrix * vec4(normal, 0.0)));

#if defined(BUMP) || defined(SPECULAR)
  eyeDir_World = cameraPos_World - vertexPos_World;
#endif

#if defined(BUMP) || defined(NORMAL)
  getTBN(normal_World, tangent, mat3(M), T, B, TBN);
#endif
}
