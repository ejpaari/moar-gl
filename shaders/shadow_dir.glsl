float calcShadow(sampler2D depthTex, vec4 pos_Light)
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
      shadow += currentDepth - bias > pcfDepth  ? 0.0 : 1.0;
    }
  }
  return shadow /= 9.0;
}
