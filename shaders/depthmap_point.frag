#version 450 core

in vec4 fragPos;

layout (location = 42) uniform int receiveShadows;
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

void main()
{
    float lightDistance = length(fragPos.xyz - lightPos);
    lightDistance = lightDistance / farPlane;
    gl_FragDepth = lightDistance;
}  