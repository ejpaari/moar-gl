layout (location = 1) in vec3 position;
layout (location = 2) in vec2 tex;

layout (std140) uniform TransformationBlock {
    mat4 M;
    mat4 V;
    mat4 MV;
    mat4 MVP;
    mat4 NormalMatrix;
};

out vec2 texCoord;

void main()
{
    texCoord = tex;
    gl_Position = MVP * vec4(position, 1.0);
}
