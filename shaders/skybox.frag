#version 450 core

in vec3 texCoord;

out vec4 outColor;

uniform samplerCube DiffuseTex;

void main()
{
    outColor = texture(DiffuseTex, texCoord);
}
