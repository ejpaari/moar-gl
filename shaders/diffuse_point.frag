#version 450 core

in vec3 normal_Cam;
in vec3 vertexPos_World;
in vec3 lightDir_Cam;
in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D DiffuseTex;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

void main()
{    
    vec3 n = normalize(normal_Cam);
    vec3 l = normalize(lightDir_Cam);
    float diff = clamp(dot(n,l), 0, 1);

    float lightDistance = length(lightPos - vertexPos_World);
    outColor = vec4(lightColor.xyz * lightColor.w * diff / (lightDistance * lightDistance), 1.0) * texture(DiffuseTex, texCoord);
}
