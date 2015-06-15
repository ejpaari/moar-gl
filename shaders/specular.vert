#version 450 core

layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 normal;

out vec3 normal_Cam;
out vec3 vertexPos_World;
out vec3 lightDir_Cam;
out vec2 texCoord;
out vec3 eyeDir_Cam;

layout (std140) uniform TransformationBlock {
    mat4 M;
    mat4 V;
    mat4 MV;
    mat4 MVP;
};

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
};

void main()
{
    normal_Cam = vec3(MV * vec4(normal, 0.0));
    vertexPos_World = vec3(M* vec4(position, 1.0));

    vec3 vertexPos_Cam = vec3(V * vec4(vertexPos_World, 1.0));
    lightDir_Cam = vec3(V * vec4(lightPos, 1.0)) - vertexPos_Cam;
    eyeDir_Cam = vec3(0.0, 0.0, 0.0) - vertexPos_Cam;

    texCoord = tex;
    gl_Position = MVP * vec4(position, 1.0);
}
