layout (location = 1) in vec3 position;

layout (std140) uniform TransformationBlock {
    mat4 M;
    mat4 V;
    mat4 MV;
    mat4 MVP;
    mat4 NormalMatrix;
};

void main()
{
    gl_Position = M * vec4(position, 1.0f);
}
