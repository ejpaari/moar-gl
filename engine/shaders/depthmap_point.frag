in vec4 fragPos;

layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

void main()
{
    gl_FragDepth = length(fragPos.xyz - lightPos) / farPlane;
}  
