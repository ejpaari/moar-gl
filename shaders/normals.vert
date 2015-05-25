#version 450 core

layout (location = 1) in vec3 position;
layout (location = 3) in vec3 normal;

out vec3 color;

layout (std140) uniform TransformationBlock {
    mat4 M;
    mat4 V;
    mat4 MV;
    mat4 MVP;
};

void main()
{
    color = normal;
    gl_Position = MVP * vec4(position, 1.0);
}
