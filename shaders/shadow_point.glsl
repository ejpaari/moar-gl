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
        shadow += currentDepth - bias > pcfDepth  ? 0.0 : 1.0;
      }
    }
    return shadow /= 9.0;
}



