layout(location = 0) out vec3 outColor;

layout (location = 12) uniform vec3 cameraPos;
layout (location = 13) uniform vec4 lightColor;
layout (location = 14) uniform vec3 lightPos;
layout (location = 15) uniform vec3 lightForward;
#if defined(POINT)
layout (location = 24) uniform samplerCube depthTex;
#else
layout (location = 24) uniform sampler2D depthTex;
#endif
layout (location = 31) uniform sampler2D colorTex;
layout (location = 32) uniform sampler2D normalTex;
layout (location = 33) uniform sampler2D positionTex;
layout (location = 41) uniform vec2 screenSize;
layout (location = 42) uniform int shadowsEnabled;
layout (location = 43) uniform float farPlane;
layout (location = 50) uniform mat4 lightSpaceProj;

#if defined(DIRECTIONAL)
in vec2 texCoord;
#endif

void main()
{
#if defined(POINT)
  vec2 texCoord = gl_FragCoord.xy / screenSize;
#endif

  outColor = vec3(0.0);
  vec3 vertexPos = texture(positionTex, texCoord).xyz;

#if defined(POINT)
  float lightDistance = length(lightPos - vertexPos);
  float lightDistSqr = lightDistance * lightDistance;
  float lightPower = lightColor.w / lightDistSqr;
#else
  float lightPower = lightColor.w;
  vec4 pos_Light = lightSpaceProj * vec4(vertexPos, 1.0);
#endif

  vec4 texColor = texture(colorTex, texCoord);
  vec3 normal = texture(normalTex, texCoord).rgb;
#if defined(POINT)  
  vec3 lightDir = normalize(lightPos - vertexPos);
#else
  vec3 lightDir = -lightForward;
#endif
  float diff = getDiffuse(normal, lightDir);

  float specular = getSpecular(normalize(cameraPos - vertexPos), lightDir, normal, texColor.a);
#if defined(POINT)
  vec3 specularComponent = vec3(specular * lightPower / lightDistSqr);
#else
  vec3 specularComponent = vec3(specular * lightPower);
#endif

  float shadow = 1.0;
  if (shadowsEnabled > 0) {
#if defined(POINT)
    shadow = calcPointShadow(depthTex, vertexPos, lightPos, farPlane);
#else
    //    shadow = calcDirShadow(depthTex, pos_Light);
    vec3 projCoords = pos_Light.xyz / pos_Light.w;
    if (projCoords.z > 1.0) {
      shadow = 1.0;
    }
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float bias = 0.005;//max(0.05 * (1.0 - dot(normal_World, lightForward)), 0.005);
    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthTex, 0);
    for(int x = -1; x <= 1; ++x) {
      for(int y = -1; y <= 1; ++y) {
        float pcfDepth = texture(depthTex, projCoords.xy + vec2(x, y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth  ? 0.0 : 0.111111;
      }
    }
    //    outColor.rgb = vec3(shadow);
#endif
  }
  
  outColor +=
    lightColor.xyz * diff * lightPower * texColor.rgb +
    specularComponent;

  outColor.rgb *= shadow;
}
