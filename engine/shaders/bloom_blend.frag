layout(location = 0) out vec3 outColor;

layout (location = 30) uniform sampler2D renderedTex;
layout (location = 31) uniform sampler2D bloomImage;

in vec2 texCoord;

void main()
{
  outColor = texture(renderedTex, texCoord).rgb + texture(bloomImage, texCoord).rgb;
}
