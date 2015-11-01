#version 450 core

in vec3 vertexPos_World;
in vec3 lightDir_Tan;
in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

void main()
{
    vec3 normal_Tan = normalize(texture(normalTex, texCoord).rgb * 2.0 - vec3(1.0));

    float diff = clamp(dot(normal_Tan, lightDir_Tan), 0, 1);

    float lightDistance = length(lightPos - vertexPos_World);
    outColor = vec4(lightColor.xyz * lightColor.w * diff / (lightDistance * lightDistance), 1.0) * texture(diffuseTex, texCoord);
}
