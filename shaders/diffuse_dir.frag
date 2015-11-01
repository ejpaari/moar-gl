#version 450 core

in vec3 normal_World;
in vec4 pos_Light;
in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 23) uniform sampler2D depthTex;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

#moar::include "../moar-gl/shaders/shadow.c"

void main()
{    
    vec3 n = normalize(normal_World);
    vec3 l = -lightForward;
    float diff = clamp(dot(n,l), 0, 1);
    float shadow = calcShadow(depthTex, pos_Light);

    outColor = shadow * vec4(lightColor.xyz * lightColor.w * diff, 1.0) * texture(diffuseTex, texCoord);
}
