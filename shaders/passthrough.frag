in vec2 texCoord;

out vec4 outColor;

layout (location = 30) uniform sampler2D renderedTex;

void main()
{
    outColor = texture(renderedTex, texCoord);
}
