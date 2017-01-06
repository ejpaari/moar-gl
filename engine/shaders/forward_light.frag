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
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

in vec2 texCoord;

in vec3 vertexPos_World;
in vec4 pos_Light;

in vec3 lightDir_World;
in vec3 normal_World;

in vec3 N;
in vec3 T;
in vec3 B;
in vec3 lightPos_Tan;
in vec3 vertexPos_Tan;

in vec3 eyeDir_World;
in vec3 eyeDir_Tan;

void main()
{
  outColor = vec4(0.0, 0.0, 0.0, 1.0);

#if defined(BUMP)
  vec3 dir = -eyeDir_World;
  vec2 step = vec2(dot(dir, normalize(T)), dot(dir, normalize(B)));
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
  vec3 lightDir_Tan = normalize(lightPos_Tan - vertexPos_Tan);
  float diff = clamp(dot(normal_Tan, lightDir_Tan), 0, 1);
#elif defined(DIFFUSE) || defined(SPECULAR)
  vec3 n = normalize(normal_World);
  #if defined(POINT)  
    vec3 l = normalize(lightDir_World);
  #else
    vec3 l = -lightForward;
  #endif
    float diff = clamp(dot(n, l), 0, 1);
#endif

#if defined(POINT)
    float shadow = calcPointShadow(depthTex, vertexPos_World, lightPos, farPlane);
#else
    float shadow = calcDirShadow(depthTex, pos_Light);
#endif

#if defined(POINT)
  float lightDistance = length(lightPos - vertexPos_World);
  float lightDistSqr = lightDistance * lightDistance;
  float lightPower = lightColor.w / lightDistSqr;
#else
  float lightPower = lightColor.w;
#endif

  outColor.rgb += shadow * lightColor.rgb * diff * lightPower * texColor.rgb;

#if defined(SPECULAR)
  #if defined(NORMAL)
  vec3 e = normalize(eyeDir_Tan);
  vec3 r = reflect(-lightDir_Tan, normal_Tan);
  #else
    vec3 e = normalize(eyeDir_World);
    vec3 r = reflect(-l, n);
  #endif
  float spec = clamp(dot(e, r), 0, 1);
  float specular = pow(spec, 10.0f) * texture(specularTex, sampleCoord).r;
  #if defined(POINT)
    outColor.rgb += vec3(specular * lightPower / lightDistSqr);
  #else
    outColor.rgb += vec3(specular * lightPower);
  #endif
#endif
  float bloom = dot(outColor.rgb, vec3(1.0));
  outBloom = vec4(vec3(0.0), 1.0);
  if (bloom > 2.0) {
    outBloom.rgb = outColor.rgb;
  }
}
