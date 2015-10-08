#version 450 core

in vec3 normal_Cam;
in vec3 normal_World;
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
    vec3 n = normalize(normal_World);
    vec3 l = -lightForward;
    float diff = clamp(dot(n,l), 0, 1);

    outColor = vec4(lightColor.xyz * lightColor.w * diff, 1.0) * texture(DiffuseTex, texCoord);
}
