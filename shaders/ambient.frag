#version 450 core

in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 10) uniform vec3 ambient;
layout (location = 11) uniform float specularity;
layout (location = 12) uniform float solidColor;
layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 42) uniform int receiveShadows;

void main()
{
    outColor = vec4(ambient, 1.0) * texture(diffuseTex, texCoord);
}
