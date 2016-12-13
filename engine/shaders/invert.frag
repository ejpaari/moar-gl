in vec2 texCoord;

out vec3 outColor;

layout (location = 30) uniform sampler2D renderedTex;

void main()
{
    outColor = vec3(1.0) - texture(renderedTex, texCoord).rgb;
}
