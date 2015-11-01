#version 450 core

layout (location = 1) in vec3 position;

layout (location = 50) uniform mat4 lightSpaceProj;
layout (location = 51) uniform mat4 lightSpaceModel;

void main()
{
    gl_Position = lightSpaceProj * lightSpaceModel * vec4(position, 1.0f);
}
