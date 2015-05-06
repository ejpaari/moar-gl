#version 450 core

layout (location = 1) in vec3 position;
layout (location = 3) in vec3 normal;

out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    color = normal;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
