#version 450 core

in vec3 position;

out vec3 Color;

uniform mat4 view;
uniform mat4 proj;

void main() {
     Color = vec3(1.0, 1.0, position.z);
     gl_Position = proj * view * vec4(position, 1.0);
}
