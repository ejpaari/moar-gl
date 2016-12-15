layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 24) uniform sampler2D specularTex;

in vec2 texCoord;
in vec3 vertexPos_World;
in vec3 normal_World;

in mat3 TBN;

bool isTransparent(float alpha)
{
  return alpha < 0.1;
}

void main()
{
  outColor = texture(diffuseTex, texCoord);

  if (isTransparent(outColor.a)) {
    discard;
  }

  outPosition = vertexPos_World;

#if defined(NORMAL)
  vec3 normal = normalize(texture(normalTex, texCoord).rgb * 2.0 - vec3(1.0));
  outNormal = normalize(TBN * normal); 
#else
  outNormal = normalize(normal_World);
#endif

#if defined(SPECULAR)
  outColor.a = texture(specularTex, texCoord).r;
#else
  outColor.a = 0.0;
#endif
}
