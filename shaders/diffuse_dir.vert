#version 450 core

layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

layout (location = 50) uniform mat4 lightSpaceProj;

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

out vec3 normal_World;
out vec4 pos_Light;
out vec2 texCoord;

void main()
{
    normal_World = vec3(M * vec4(normal, 0.0));
    texCoord = tex;
// Todo: maybe if (!receiveShadows) then skip matrix calculation?
    pos_Light = lightSpaceProj * M * vec4(position, 1.0);
    gl_Position = MVP * vec4(position, 1.0);
}
