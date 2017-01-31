layout(location = 0) out vec4 outColor;

layout (location = 16) uniform int numLights;
layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 22) uniform sampler2D bumpTex;
layout (location = 23) uniform sampler2D specularTex;
layout (location = 43) uniform float farPlane;

uniform sampler2D depthTexs[MAX_NUM_SHADOWMAPS];

layout (location = 70) uniform int enableShadows[MAX_NUM_LIGHTS_PER_TYPE];

layout (std140) uniform LightBlock {
  vec4 bLightColor[MAX_NUM_LIGHTS_PER_TYPE];
  vec3 bLightPos[MAX_NUM_LIGHTS_PER_TYPE];
  vec3 bLightForward[MAX_NUM_LIGHTS_PER_TYPE];
};

in vec2 texCoord;
in vec3 vertexPos_World;
in vec3 normal_World;
in vec3 eyeDir_World;
in vec4 pos_Light[MAX_NUM_LIGHTS_PER_TYPE];
in vec3 T;
in vec3 B;
in mat3 TBN;

void main()
{
  outColor = vec4(0.0, 0.0, 0.0, 1.0);

  vec3 normNormal_World = normalize(normal_World);

#if defined(BUMP)
  vec2 sampleCoord = getBumpedTexCoord(eyeDir_World, T, B, bumpTex, texCoord);
#else
  vec2 sampleCoord = texCoord;
#endif    

#if defined(NORMAL)
  normNormal_World = getWorldSpaceNormal(normalTex, sampleCoord, TBN);
#endif

  vec4 texColor = texture(diffuseTex, sampleCoord);
  if (isTransparent(texColor.a)) {
    discard;
  }

  vec3 normEyeDir_World = normalize(eyeDir_World);
  float specularPower = texture(specularTex, sampleCoord).r;

  // Start light loop
  for (int i = 0; i < numLights; ++i) {
    vec3 lightDir_World = -bLightForward[i];

#if defined(DIFFUSE)
    float diff = getDiffuse(normNormal_World, lightDir_World);
#else
    texColor = vec4(1.0);
    float diff = 1.0;
#endif

    float lightPower = bLightColor[i].w;

    vec3 specularComponent = vec3(0.0);
#if defined(SPECULAR)    
    float specular = getSpecular(normEyeDir_World, lightDir_World, normNormal_World, specularPower);
    specularComponent = vec3(specular * lightPower);
#endif

    float shadow = 1.0;
    // Shadow functions had to be inlined since passing an array element depthTexs[i]
    if (enableShadows[i] > 0) {
      // shadow = calcDirShadow(depthTex, pos_Light);    
      shadow = 0.0;      
      vec3 projCoords = pos_Light[i].xyz / pos_Light[i].w;
      if (projCoords.z > 1.0) {
        shadow = 1.0;
      } else {
        projCoords = projCoords * 0.5 + 0.5;
        float currentDepth = projCoords.z;
        float bias = 0.005;//max(0.05 * (1.0 - dot(normal_World, lightForward)), 0.005);
        vec2 texelSize = 1.0 / textureSize(depthTexs[i], 0);
        for(int x = -1; x <= 1; ++x) {
          for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(depthTexs[i], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 0.0 : 0.111111;
          }
        }
      }
    }

    vec3 add =       
      bLightColor[i].rgb * diff * lightPower * texColor.rgb +
      specularComponent;
    add *= shadow;

    outColor.rgb += add;
  }
}
