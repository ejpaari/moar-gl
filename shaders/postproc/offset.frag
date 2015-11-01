#version 450 core

in vec2 texCoord;

out vec4 outColor;

layout (location = 30) uniform sampler2D renderedTex;
layout (location = 40) uniform float time;
layout (location = 41) uniform vec2 screenSize;


void main()
{
    outColor = texture(renderedTex, texCoord + 0.002 * vec2(sin(time + screenSize.x * texCoord.x),
                                                            cos(time + screenSize.y * texCoord.y)));
}
