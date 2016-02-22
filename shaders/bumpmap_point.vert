#version 450 core

layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

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

out vec3 vertexPos_World;
out vec3 lightDir_Tan;
out vec2 texCoord;
out vec3 N;
out vec3 T;
out vec3 B;
out vec3 eyeDir_Cam;

void main()
{
    vertexPos_World = vec3(M * vec4(position, 1.0));

    N = mat3(MV) * normal;
    T = mat3(MV) * tangent;
    B = cross(T, N);

    vec3 vertexPos_Cam = vec3(MV * vec4(position, 1.0));
    vec3 L = vec3(V * vec4(lightPos, 1.0)) - vertexPos_Cam;
    lightDir_Tan = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));

    eyeDir_Cam = normalize(vec3(0.0) - vertexPos_Cam);

    texCoord = tex;
    gl_Position = MVP * vec4(position, 1.0);
}
