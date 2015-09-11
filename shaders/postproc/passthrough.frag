#version 450 core

in vec2 texCoord;

out vec4 outColor;

layout (location = 30) uniform sampler2D RenderedTex;

void main()
{
    outColor = texture(RenderedTex, texCoord);
}
