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
layout (location = 42) uniform int shadowsEnabled;
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

in vec2 texCoord;

in vec3 vertexPos_World;
in vec4 pos_Light;

in vec3 pointLightDir_World;
in vec3 normal_World;

in mat3 TBN;
in vec3 T;
in vec3 B;

in vec3 eyeDir_World;

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
  vec3 bumpDir = -normalize(eyeDir_World);
  vec2 step = vec2(dot(bumpDir, normalize(T)), dot(bumpDir, normalize(B)));
  float stepDepth = BUMP_DEPTH / NUM_BUMP_STEPS;
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

#if defined(NORMAL)
  vec3 normal_Tan = normalize(texture(normalTex, sampleCoord).rgb * 2.0 - vec3(1.0));
  normNormal_World = normalize(TBN * normal_Tan);
  float diff = clamp(dot(normNormal_World, lightDir_World), 0, 1);
#elif defined(DIFFUSE)
  float diff = clamp(dot(normNormal_World, lightDir_World), 0, 1);
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
  vec3 e = normalize(eyeDir_World);
  vec3 r = reflect(-lightDir_World, normNormal_World);
  float spec = clamp(dot(e, r), 0, 1);
  float specular = pow(spec, 10.0f) * texture(specularTex, sampleCoord).r;
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

  float bloom = dot(outColor.rgb, vec3(1.0));
  outBloom = vec4(vec3(0.0), 1.0);
  if (bloom > 2.0) {
    outBloom.rgb = outColor.rgb;
  }
}
