layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outColor;

layout (location = 12) uniform vec3 cameraPos_World;
layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 22) uniform sampler2D bumpTex;
layout (location = 24) uniform sampler2D specularTex;
layout (location = 42) uniform int receiveShadows;
layout (location = 43) uniform float farPlane;

in vec2 texCoord;
in vec3 vertexPos_World;
in vec3 normal_World;

in mat3 TBN;
in vec3 T;
in vec3 B;
in vec3 eyeDir_World;

void main()
{
#if defined(BUMP)
  vec3 dir = -eyeDir_World;
  vec2 step = vec2(dot(dir, normalize(T)), dot(dir, normalize(B)));
  float stepDepth = BUMP_DEPTH / NUM_BUMP_STEPS;
  float currentDepth = 0.0;
  float height = texture(bumpTex, texCoord + currentDepth * step).r * BUMP_DEPTH;

  while ((BUMP_DEPTH - currentDepth) > height) {
    height = texture(bumpTex, texCoord + currentDepth * step).r * BUMP_DEPTH;
    currentDepth += stepDepth;
  }
  vec2 sampleCoord = texCoord + currentDepth * step;
#else
  vec2 sampleCoord = texCoord;
#endif    

  outColor = texture(diffuseTex, sampleCoord);

  if (isTransparent(outColor.a)) {
    discard;
  }

  outPosition.xyz = vertexPos_World;
  outPosition.w = length(vertexPos_World - cameraPos_World) / farPlane;

#if defined(NORMAL)
  vec3 normal = normalize(texture(normalTex, sampleCoord).rgb * 2.0 - vec3(1.0));
  outNormal.xyz = normalize(TBN * normal); 
#else
  outNormal.xyz = normalize(normal_World);
#endif

  outNormal.a = 0.0f;
  if (receiveShadows != 0) {
    outNormal.a = 1.0f;
  }

#if defined(SPECULAR)
  outColor.a = texture(specularTex, sampleCoord).r;
#else
  outColor.a = 0.0;
#endif
}
