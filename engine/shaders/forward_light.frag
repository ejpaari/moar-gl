layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 22) uniform sampler2D bumpTex;
#if defined(POINT)
layout (location = 23) uniform samplerCube depthTex;
#else
layout (location = 23) uniform sampler2D depthTex;
#endif
layout (location = 24) uniform sampler2D specularTex;
layout (location = 42) uniform int receiveShadows;
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

in vec2 texCoord;

in vec3 vertexPos_World;
in vec4 pos_Light;

in vec3 normal_Cam;
in vec3 lightDir_Cam;
in vec3 normal_World;

in vec3 N;
in vec3 T;
in vec3 B;
in vec3 lightDir_Tan;

in vec3 eyeDir_Cam;
in vec3 eyeDir_Tan;

const float BUMP_DEPTH = 0.025;
const int NUM_STEPS = 50;

float calcPointShadow(samplerCube depthTex, vec3 vertexPos_World, vec3 lightPos_World, float farClipDistance)
{
  vec3 vertexToLight = vertexPos_World - lightPos_World;
  float currentDepth = length(vertexToLight);
  float bias = 0.02;

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
  outColor = vec4(0.0);
#if defined(POINT)
  float lightDistance = length(lightPos - vertexPos_World);
  float lightDistSqr = lightDistance * lightDistance;
  float lightPower = lightColor.w / lightDistSqr;
#else
  float lightPower = lightColor.w;
#endif

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
#elif defined(DIFFUSE) || defined(SPECULAR)
  #if defined(POINT)
    vec3 n = normalize(normal_Cam);
    vec3 l = normalize(lightDir_Cam);
  #else
    vec3 n = normal_World;
    vec3 l = -lightForward;
  #endif
  float diff = clamp(dot(n, l), 0, 1);
#endif

  float shadow = 1.0;
  if (receiveShadows != 0) {
#if defined(POINT)
    shadow = calcPointShadow(depthTex, vertexPos_World, lightPos, farPlane);
#else
    shadow = calcDirShadow(depthTex, pos_Light);
#endif
  }

  outColor += shadow * vec4(lightColor.xyz * diff * lightPower, 1.0) * texColor;

#if defined(SPECULAR)
  #if defined(NORMAL)
    vec3 e = eyeDir_Tan;
    vec3 r = reflect(-lightDir_Tan, normal_Tan);
  #else
    vec3 e = eyeDir_Cam;
    vec3 r = reflect(-l, n);
  #endif
  float spec = clamp(dot(e, r), 0, 1);
  float specular = pow(spec, 10.0f) * texture(specularTex, sampleCoord).r;
  #if defined(POINT)
    outColor += vec4(vec3(specular * lightPower / lightDistSqr), 1.0);
  #else
    outColor += vec4(vec3(specular * lightPower), 1.0);
  #endif
#endif
  float bloom = dot(outColor.rgb, vec3(1.0));
  outBloom = vec4(vec3(0.0), 1.0);
  if (bloom > 2.0) {
    outBloom = vec4(outColor.rgb, 1.0);
  }
}
