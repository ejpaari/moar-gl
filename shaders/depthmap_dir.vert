#version 450 core

layout (location = 1) in vec3 position;

layout (location = 50) uniform mat4 lightSpaceProj;

layout (std140) uniform TransformationBlock {
    mat4 M;
    mat4 V;
    mat4 MV;
    mat4 MVP;
};

void main()
{
    gl_Position = lightSpaceProj * M * vec4(position, 1.0f);
}
