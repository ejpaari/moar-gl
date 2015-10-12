#version 450 core

layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

out vec3 vertexPos_World;
out vec3 lightDir_Tan;
out vec3 eyeDir_Tan;
out vec2 texCoord;

layout (std140) uniform TransformationBlock {
    mat4 M;
    mat4 V;
    mat4 MV;
    mat4 MVP;
};

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

void main()
{
    vec3 N = normalize(mat3(M) * normal);
    vec3 T = normalize(mat3(M) * tangent);
    vec3 B = cross(N, T);

    vec3 L = -lightForward;
    lightDir_Tan = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));

    texCoord = tex;
    gl_Position = MVP * vec4(position, 1.0);
}
