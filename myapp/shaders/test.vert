#version 450 core

in vec3 position;
in vec3 normal;
in vec2 tex;

out vec3 Color;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    Color = vec3(proj * view * model * vec4(normal, 0.0)) * normal;
    TexCoord = tex;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
