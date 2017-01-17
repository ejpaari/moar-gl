layout(location = 0) out vec3 outColor;

layout (location = 30) uniform sampler2D renderedTex;

in vec2 texCoord;

void main()
{
  vec3 texColor = texture(renderedTex, texCoord).rgb;
  float bloom = dot(texColor, vec3(1.0));
  outColor = bloom > 2.0 ? texColor : vec3(0.0);
}
