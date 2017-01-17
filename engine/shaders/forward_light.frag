layout(location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 22) uniform sampler2D bumpTex;
#if defined(POINT)
layout (location = 23) uniform samplerCube depthTex;
#else
layout (location = 23) uniform sampler2D depthTex;
#endif
layout (location = 24) uniform sampler2D specularTex;
layout (location = 42) uniform int shadowsEnabled;
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

in vec2 texCoord;
in vec3 vertexPos_World;
in vec3 normal_World;
in vec3 eyeDir_World;
in vec4 pos_Light;
in vec3 pointLightDir_World;
in vec3 T;
in vec3 B;
in mat3 TBN;

void main()
{
  outColor = vec4(0.0, 0.0, 0.0, 1.0);
  vec3 normNormal_World = normalize(normal_World);
#if defined(POINT)
  vec3 lightDir_World = normalize(pointLightDir_World);
#else
  vec3 lightDir_World = -lightForward;
#endif

#if defined(BUMP)
  vec2 sampleCoord = getBumpedTexCoord(eyeDir_World, T, B, bumpTex, texCoord);
#else
  vec2 sampleCoord = texCoord;
#endif    

#if defined(NORMAL)
  normNormal_World = getWorldSpaceNormal(normalTex, sampleCoord, TBN);
#endif

#if defined(DIFFUSE)
  float diff = getDiffuse(normNormal_World, lightDir_World);
#endif

#if defined(POINT)
  float lightDistance = length(lightPos - vertexPos_World);
  float lightDistSqr = lightDistance * lightDistance;
  float lightPower = lightColor.w / lightDistSqr;
#else
  float lightPower = lightColor.w;
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

  vec3 specularComponent = vec3(0.0);
#if defined(SPECULAR)
  float power = texture(specularTex, sampleCoord).r;
  float specular = getSpecular(normalize(eyeDir_World), lightDir_World, normNormal_World, power);
  #if defined(POINT)
    specularComponent = vec3(specular * lightPower / lightDistSqr);
  #else
    specularComponent = vec3(specular * lightPower);
  #endif
#endif

  float shadow = 1.0;
  if (shadowsEnabled > 0) {
#if defined(POINT)
    shadow = calcPointShadow(depthTex, vertexPos_World, lightPos, farPlane);
#else
    shadow = calcDirShadow(depthTex, pos_Light);
#endif
  }

  outColor.rgb += 
    lightColor.rgb * diff * lightPower * texColor.rgb +
    specularComponent;

  outColor.rgb *= shadow;
}
