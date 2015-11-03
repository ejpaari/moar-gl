#version 450 core

in vec3 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform samplerCube diffuseTex;

void main()
{
    outColor = texture(diffuseTex, texCoord);
}
