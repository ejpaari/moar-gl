in vec3 vertexPos_World;
in vec2 texCoord;

#if defined(DIFFUSE) || defined(SPECULAR)
in vec3 normal_Cam;
in vec3 lightDir_Cam;
#endif

#if defined(BUMP) || defined(NORMAL)
in vec3 N;
in vec3 T;
in vec3 B;
in vec3 eyeDir_Cam;
in vec3 lightDir_Tan;
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

layout (location = 23) uniform samplerCube depthTex;
layout (location = 42) uniform int receiveShadows;
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

#if defined(BUMP)
const float BUMP_DEPTH = 0.025;
const int NUM_STEPS = 50;
#endif

float calcPointShadow(samplerCube depthTex, vec3 vertexPos_World, vec3 lightPos_World, float farClipDistance)
{
  vec3 vertexToLight = vertexPos_World - lightPos_World;
  float currentDepth = length(vertexToLight);
  float bias = 0.05;

  float shadow = 0.0;
  vec2 texelSize = 2.0 * length(vertexToLight) / textureSize(depthTex, 0);
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      vec3 dir = vertexToLight + vec3(texelSize.x * x, texelSize.y * y, 0.0);
      float pcfDepth = texture(depthTex, dir).r * farClipDistance;
      shadow += currentDepth - bias > pcfDepth  ? 0.0 : 0.111111;
    }
  }
  return shadow;
}

bool isTransparent(float alpha)
{
  return alpha < 0.1;
}

bool isTooFar(float lightPower)
{
  return lightPower < 0.05;
}

void main()
{
  float lightDistance = length(lightPos - vertexPos_World);
  float lightPower = lightColor.w / (lightDistance * lightDistance);

  if (isTooFar(lightPower)) {
    discard;
  }

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

  vec4 texColor = texture(diffuseTex, sampleCoord);

  if (isTransparent(texColor.a)) {
    discard;
  }

#if defined(NORMAL)
  vec3 normal_Tan = normalize(texture(normalTex, sampleCoord).rgb * 2.0 - vec3(1.0));
  float diff = clamp(dot(normal_Tan, lightDir_Tan), 0, 1);
#elif defined(DIFFUSE) || defined(SPECULAR)
  vec3 n = normalize(normal_Cam);
  vec3 l = normalize(lightDir_Cam);
  float diff = clamp(dot(n,l), 0, 1);
#endif

  float shadow = receiveShadows != 0 ? 
    calcPointShadow(depthTex, vertexPos_World, lightPos, farPlane) : 
    1.0;

  outColor = shadow * vec4(lightColor.xyz * diff * lightPower, 1.0) * texColor;
}
