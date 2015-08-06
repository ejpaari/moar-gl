#version 450 core

in vec2 texCoord;

out vec4 outColor;

layout (location = 30) uniform sampler2D RenderedTex;
layout (location = 40) uniform float Time;
layout (location = 41) uniform vec2 ScreenSize;


void main()
{
    outColor = texture(RenderedTex,
                       texCoord + 0.005 * vec2(sin(Time + ScreenSize.x * texCoord.x),
                                               cos(Time + ScreenSize.y * texCoord.y)));
}
