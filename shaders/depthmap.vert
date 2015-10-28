#version 450 core

layout (location = 1) in vec3 position;

layout (location = 50) uniform mat4 LightSpaceProj;
layout (location = 51) uniform mat4 LightSpaceModel;

void main()
{
    gl_Position = LightSpaceProj * LightSpaceModel * vec4(position, 1.0f);
}
