#version 450 core

layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

out vec3 Color;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    Color = normal;
    TexCoord = tex;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
