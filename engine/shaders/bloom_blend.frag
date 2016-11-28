in vec2 texCoord;

out vec4 outColor;

layout (location = 30) uniform sampler2D renderedTex;
layout (location = 31) uniform sampler2D bloomImage;

void main()
{
  vec3 result = texture(renderedTex, texCoord).rgb + texture(bloomImage, texCoord).rgb;
  outColor = vec4(result, 1.0);
}
