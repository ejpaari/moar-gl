#version 450 core

in vec3 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform samplerCube DiffuseTex;

void main()
{
    outColor = texture(DiffuseTex, texCoord);
}
