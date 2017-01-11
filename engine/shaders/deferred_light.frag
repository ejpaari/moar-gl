layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outBloom;

layout (location = 12) uniform vec3 cameraPos;
#if defined(POINT)
layout (location = 23) uniform samplerCube depthTex;
#else
layout (location = 23) uniform sampler2D depthTex;
#endif
layout (location = 30) uniform sampler2D colorTex;
layout (location = 31) uniform sampler2D normalTex;
layout (location = 32) uniform sampler2D positionTex;
layout (location = 41) uniform vec2 screenSize;
layout (location = 42) uniform int shadowsEnabled;
layout (location = 43) uniform float farPlane;
layout (location = 50) uniform mat4 lightSpaceProj;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

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
  float diff = clamp(dot(normal, lightDir), 0, 1);

  vec3 e = normalize(cameraPos - vertexPos);
  vec3 r = reflect(-lightDir, normal);
  float spec = clamp(dot(e, r), 0, 1);
  float specular = pow(spec, 10.0f) * texColor.a;
  
  float shadow = 1.0;
  if (shadowsEnabled > 0) {
#if defined(POINT)
    shadow = calcPointShadow(depthTex, vertexPos, lightPos, farPlane);
#else
    shadow = calcDirShadow(depthTex, pos_Light);
#endif
  }

#if defined(POINT)
  vec3 specularComponent = vec3(specular * lightPower / lightDistSqr);
#else
  vec3 specularComponent = vec3(specular * lightPower);
#endif
  
  outColor +=
    lightColor.xyz * diff * lightPower * texColor.rgb +
    specularComponent;

  outColor.rgb *= shadow;

  float bloom = dot(outColor.rgb, vec3(1.0));
  outBloom = vec3(0.0);
  if (bloom > 2.0) {
    outBloom.rgb = outColor.rgb;
  }
}
