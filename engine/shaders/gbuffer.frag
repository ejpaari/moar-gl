layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec3 outVSPosition;

layout (location = 12) uniform vec3 cameraPos_World;
layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 22) uniform sampler2D bumpTex;
layout (location = 24) uniform sampler2D specularTex;
layout (location = 43) uniform float farPlane;

in vec2 texCoord;
in vec3 vertexPos_World;
in vec3 vertexPos_View;
in vec3 normal_World;
in vec3 eyeDir_World;
in vec3 T;
in vec3 B;
in mat3 TBN;

void main()
{
#if defined(BUMP)
  vec2 sampleCoord = getBumpedTexCoord(eyeDir_World, T, B, bumpTex, texCoord);
#else
  vec2 sampleCoord = texCoord;
#endif    

  outColor = texture(diffuseTex, sampleCoord);

  if (isTransparent(outColor.a)) {
    discard;
  }

  outPosition.xyz = vertexPos_World;

#if defined(NORMAL)
  outNormal.xyz = getWorldSpaceNormal(normalTex, sampleCoord, TBN);
#else
  outNormal.xyz = normalize(normal_World);
#endif

#if defined(SPECULAR)
  outColor.a = texture(specularTex, sampleCoord).r;
#else
  outColor.a = 0.0;
#endif

  outVSPosition = vertexPos_View;
}
