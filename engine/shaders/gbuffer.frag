layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;

in vec2 texCoord;
in vec3 vertexPos_World;
in vec3 normal_World;

void main()
{
  outPosition = vertexPos_World;
  outNormal = normalize(normal_World);
  outColor.rgb = texture(diffuseTex, texCoord).rgb;
  outColor.a = 0.0;
}
