in vec2 texCoord;
in vec4 pos_Light;

#if defined(DIFFUSE) || defined(SPECULAR)
in vec3 normal_World;
#endif

#if defined(BUMP) || defined(NORMAL)
in vec3 N;
in vec3 T;
in vec3 B;
in vec3 lightDir_Tan;
#endif

#if defined(BUMP)
in vec3 eyeDir_Cam;
#endif

layout(location = 0) out vec4 outColor;

#if defined(DIFFUSE)
layout (location = 20) uniform sampler2D diffuseTex;
#endif

#if defined(NORMAL)
layout (location = 21) uniform sampler2D normalTex;
#endif

#if defined(BUMP)
layout (location = 22) uniform sampler2D bumpTex;
#endif

layout (location = 23) uniform sampler2D depthTex;
layout (location = 42) uniform int receiveShadows;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

#if defined(BUMP)
const float BUMP_DEPTH = 0.025;
const int NUM_STEPS = 50;
#endif

float calcDirShadow(sampler2D depthTex, vec4 pos_Light)
{
  vec3 projCoords = pos_Light.xyz / pos_Light.w;
  if (projCoords.z > 1.0) {
    return 1.0;
  }
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(depthTex, projCoords.xy).r;
  float currentDepth = projCoords.z;
  float bias = 0.005;//max(0.05 * (1.0 - dot(normal_World, lightForward)), 0.005);

  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(depthTex, 0);
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(depthTex, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += currentDepth - bias > pcfDepth  ? 0.0 : 0.111111;
    }
  }
  return shadow;
}

bool isTransparent(float alpha)
{
  return alpha < 0.1;
}

void main()
{
#if defined(BUMP)
  vec3 dir = -eyeDir_Cam;
  vec2 step = vec2(dot(dir, normalize(T)), dot(dir, normalize(B)));
  float stepDepth = BUMP_DEPTH / NUM_STEPS;
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

#if defined(DIFFUSE)
  vec4 texColor = texture(diffuseTex, sampleCoord);

  if (isTransparent(texColor.a)) {
    discard;
  }
#else
  vec4 texColor = vec4(1.0);
  float diff = 1.0;
#endif

#if defined(NORMAL)
  vec3 normal_Tan = normalize(texture(normalTex, sampleCoord).rgb * 2.0 - vec3(1.0));
  float diff = clamp(dot(normal_Tan, lightDir_Tan), 0, 1);
#elif defined(DIFFUSE)
  vec3 n = normal_World;
  vec3 l = -lightForward;
  float diff = clamp(dot(n,l), 0, 1);
#endif

  float shadow = receiveShadows != 0 ? calcDirShadow(depthTex, pos_Light) : 1.0;

  outColor = shadow * vec4(lightColor.xyz * diff * lightColor.w, 1.0) * texColor;
}
