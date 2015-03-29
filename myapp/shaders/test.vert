#version 450 core

in vec3 position;
in vec3 normal;

out vec3 Color;

uniform mat4 view;
uniform mat4 proj;

void main() {
     Color = normal;
     gl_Position = proj * view * vec4(position, 1.0);
}
