
// begin common.frag

const float BUMP_DEPTH = 0.025;
const int NUM_BUMP_STEPS = 50;

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

// end common.frag 

