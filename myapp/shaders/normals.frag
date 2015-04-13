#version 450 core

in vec3 Color;
in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D DiffuseTex;

void main()
{
    outColor = vec4(Color, 1.0);
}
