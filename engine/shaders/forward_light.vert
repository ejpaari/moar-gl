layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

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

out vec3 normal_Cam;
out vec3 lightDir_Cam;
out vec3 normal_World;

out vec3 N;
out vec3 T;
out vec3 B;
out vec3 lightDir_Tan;

out vec3 eyeDir_Cam;
out vec3 eyeDir_Tan;

void main()
{
  gl_Position = MVP * vec4(position, 1.0);
  texCoord = tex;
  vec3 vertexPos_Cam = vec3(MV * vec4(position, 1.0));
  
#if defined(POINT)
  vertexPos_World = vec3(M * vec4(position, 1.0));
#else
  pos_Light = lightSpaceProj * M * vec4(position, 1.0);
#endif

#if defined(DIFFUSE) || defined(SPECULAR)
  #if defined(POINT)
    normal_Cam = vec3(MV * vec4(normal, 0.0));
    lightDir_Cam = vec3(V * vec4(lightPos, 1.0)) - vertexPos_Cam;
  #else
    normal_World = normalize(vec3(M * vec4(normal, 0.0)));
  #endif
#endif

#if defined(BUMP) || defined(NORMAL)
  N = mat3(M) * normal;
  T = mat3(M) * tangent;
  B = cross(T, N);
  #if defined(POINT)
    vec3 L = lightPos - vertexPos_World;
  #else
    vec3 L = -lightForward;
  #endif
  lightDir_Tan = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
#endif

#if defined(BUMP) || defined(SPECULAR)
  eyeDir_Cam = normalize(vec3(0.0) - vertexPos_Cam);
#endif

#if defined(SPECULAR) && defined(NORMAL)
  eyeDir_Tan = normalize(vec3(dot(eyeDir_Cam, T), dot(eyeDir_Cam, B), dot(eyeDir_Cam, N)));
#endif
}
