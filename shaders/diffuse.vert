#version 450 core

layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

out vec3 normal_Cam;
out vec3 vertexPos_World;
out vec3 lightDir_Cam;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 lightPos;

void main()
{
    normal_Cam = vec3(view * model * vec4(normal, 0.0));
    vertexPos_World = vec3(model * vec4(position, 1.0));

    vec3 vertexPos_Cam = vec3(view * vec4(vertexPos_World, 1.0));
    lightDir_Cam = vec3(view * vec4(lightPos, 1.0)) - vertexPos_Cam;

    texCoord = tex;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
