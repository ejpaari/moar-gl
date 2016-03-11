layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;
#if defined(BUMP) || defined(NORMAL)
layout (location = 4) in vec3 tangent;
#endif

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

out vec3 vertexPos_World;
out vec2 texCoord;

#if defined(DIFFUSE) || defined(SPECULAR)
out vec3 normal_Cam;
out vec3 lightDir_Cam;
#endif

#if defined(BUMP) || defined(NORMAL)
out vec3 N;
out vec3 T;
out vec3 B;
out vec3 eyeDir_Cam;
out vec3 lightDir_Tan;
#endif

void main()
{
  gl_Position = MVP * vec4(position, 1.0);
  vertexPos_World = vec3(M * vec4(position, 1.0));
  texCoord = tex;
  vec3 vertexPos_Cam = vec3(MV * vec4(position, 1.0));

#if defined(DIFFUSE) || defined(SPECULAR)
  normal_Cam = vec3(MV * vec4(normal, 0.0));
  lightDir_Cam = vec3(V * vec4(lightPos, 1.0)) - vertexPos_Cam;
#endif

#if defined(BUMP) || defined(NORMAL)
  N = mat3(MV) * normal;
  T = mat3(MV) * tangent;
  B = cross(T, N);
  eyeDir_Cam = normalize(vec3(0.0) - vertexPos_Cam);
  vec3 L = vec3(V * vec4(lightPos, 1.0)) - vertexPos_Cam;
  lightDir_Tan = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
#endif
}
