
// begin common.frag

const float BUMP_DEPTH = 0.025;
const int NUM_BUMP_STEPS = 50;
const float STEP_DEPTH = BUMP_DEPTH / NUM_BUMP_STEPS;

vec2 getBumpedTexCoord(vec3 eyeDir_World, vec3 T, vec3 B, sampler2D bumpTex, vec2 texCoord)
{
  vec3 bumpDir = -normalize(eyeDir_World);
  vec2 step = vec2(dot(bumpDir, normalize(T)), dot(bumpDir, normalize(B)));
  float currentDepth = 0.0;
  float height = texture(bumpTex, texCoord + currentDepth * step).r * BUMP_DEPTH;

  while ((BUMP_DEPTH - currentDepth) > height) {
    height = texture(bumpTex, texCoord + currentDepth * step).r * BUMP_DEPTH;
    currentDepth += STEP_DEPTH;
  }
  return texCoord + currentDepth * step;
}

vec3 getWorldSpaceNormal(sampler2D normalTex, vec2 texCoord, mat3 TBN)
{
  return normalize(TBN * normalize(texture(normalTex, texCoord).rgb * 2.0 - vec3(1.0)));
}

float getDiffuse(vec3 normal, vec3 lightDir) 
{
  return clamp(dot(normal, lightDir), 0, 1);
}

float getSpecular(vec3 eyeDir, vec3 lightDir, vec3 normal, float power)
{
  float spec = clamp(dot(eyeDir, reflect(-lightDir, normal)), 0, 1);
  return pow(spec, 10.0f) * power;
}

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

vec3 getBloom(vec3 outColor)
{
  float bloom = dot(outColor.rgb, vec3(1.0));
  if (bloom > 2.0) {
    return outColor;
  }
  return vec3(0.0);
}

// end common.frag 

