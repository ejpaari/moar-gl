layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;

in vec2 texCoord;
in vec3 vertexPos_World;
in vec3 normal_World;

in mat3 TBN;

void main()
{
  outPosition = vertexPos_World;
#if defined(NORMAL)
  vec3 normal = texture(normalTex, texCoord).rgb;
  normal = normalize(normal * 2.0 - 1.0);   
  outNormal = normalize(TBN * normal); 
#else
  outNormal = normalize(normal_World);
#endif
  outColor.rgb = texture(diffuseTex, texCoord).rgb;
  outColor.a = 0.0;
}
