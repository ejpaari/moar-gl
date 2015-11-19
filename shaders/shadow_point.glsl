float calcPointShadow(samplerCube depthTex, vec3 vertexPos_World, vec3 lightPos_World, float farClipDistance)
{
    vec3 vertexToLight = vertexPos_World - lightPos_World; 
    float closestDepth = texture(depthTex, vertexToLight).r;
    closestDepth *= farClipDistance;
    float currentDepth = length(vertexToLight);
    float bias = 0.05; 
    return currentDepth - bias > closestDepth ? 0.0 : 1.0;
}