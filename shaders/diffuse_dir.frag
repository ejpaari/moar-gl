#version 450 core

in vec3 normal_World;
in vec4 pos_Light;
in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D DiffuseTex;
layout (location = 23) uniform sampler2D DepthTex;

layout (location = 50) uniform mat4 LightSpaceProj;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

void main()
{    
    vec3 n = normalize(normal_World);
    vec3 l = -lightForward;
    float diff = clamp(dot(n,l), 0, 1);

    vec3 projCoords = pos_Light.xyz / pos_Light.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(DepthTex, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = 0.005;//max(0.05 * (1.0 - dot(normal_World, lightForward)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;

    outColor = shadow * vec4(lightColor.xyz * lightColor.w * diff, 1.0) * texture(DiffuseTex, texCoord);
}
