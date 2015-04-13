#version 450 core

in vec3 Color;
in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D DiffuseTex;
uniform sampler2D NormalTex;

void main()
{
    outColor = (0.4 * texture(NormalTex, TexCoord) + 0.6 * texture(DiffuseTex, TexCoord));
}
